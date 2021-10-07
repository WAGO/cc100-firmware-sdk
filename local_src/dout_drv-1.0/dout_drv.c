// SPDX-License-Identifier: GPL-2.0
//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, version 2.
///
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program. If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

///------------------------------------------------------------------------------
/// \file    dout_drv.c
///
/// \brief   Driver for digital OUT on CC100 and similar devices
///
/// \author  Ralf Gliese, elrest Automationssysteme GmbH
///          Tomas Marek, elrest Automationssysteme GmbH
///
/// Expose digial IOs using sysfs interface:
///   /sys/kernel/dout_drv/DOUT_DATA
///
/// Dedicated driver is used instead of GPIOs from user space to provide
/// consistent interface with PCB pins to bit number maping implemented in
/// Kernel.
///
/// DTS binding example:
///
///       dout_drv {
///                compatible = "elrest,dout_drv";
///                douts-gpios = <&gpioh 10 GPIO_ACTIVE_HIGH>,     /* bit 0 */
///                        <&gpioi 8  GPIO_ACTIVE_HIGH>,
///                        <&gpioi 2  GPIO_ACTIVE_HIGH>,
///                        <&gpioi 6  GPIO_ACTIVE_HIGH>;
///                leds-gpios = <&gpioj 15 GPIO_ACTIVE_HIGH>,      /* bit 0 LED */
///                        <&gpioj 12 GPIO_ACTIVE_HIGH>,
///                        <&gpiok 5 GPIO_ACTIVE_HIGH>,
///                        <&gpiok 4 GPIO_ACTIVE_HIGH>;
///                status = "okay";
///        };
///
/// The leds-gpios is optional.
///
///------------------------------------------------------------------------------

#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>

struct dout_drv_gpios {
	struct gpio_desc *out;
	struct gpio_desc *led;
};

struct dout_drv_priv {
	unsigned long dout_data;
	unsigned int pin_cnt;
	struct mutex lock;
	struct kobject *dout_drv_kobj;
	struct dout_drv_gpios gpios[];
};

static struct dout_drv_priv *drv_priv;

static ssize_t dout_data_show(struct kobject *kobj, struct kobj_attribute *attr,
	char *buf)
{
	return sprintf(buf, "%lu\n", drv_priv->dout_data);
}

static ssize_t dout_data_store(struct kobject *kobj, struct kobj_attribute *attr,
				const char *buf, size_t count)
{
	unsigned long dout_data;
	unsigned int i;
	int ret;

	if (kstrtoul(buf, 0, &dout_data))
		return -EINVAL;

	/* check the value is not greater than max pins */
	if (dout_data > (1 << drv_priv->pin_cnt))
		return -EINVAL;

	ret = mutex_lock_interruptible(&drv_priv->lock);
	if (ret)
		return ret;

	drv_priv->dout_data = dout_data;
	for (i = 0; i < drv_priv->pin_cnt; i++) {
		unsigned long dout = (dout_data & (1 << i) ? 1 : 0);

		gpiod_set_value(drv_priv->gpios[i].out, dout);
		if (drv_priv->gpios[i].led)
			gpiod_set_value(drv_priv->gpios[i].led, dout);
	}

	mutex_unlock(&drv_priv->lock);
	return count;
}

static struct kobj_attribute dout_drv_attribute =
	__ATTR(DOUT_DATA, S_IWUSR | S_IRUGO, dout_data_show, dout_data_store);

static int dout_drv_probe(struct platform_device *pdev)
{
	int pins = gpiod_count(&pdev->dev, "douts");
	unsigned int i;
	int error;

	dev_info(&pdev->dev, "digital output driver init (pins %d)\n", pins);

	if (pins < 0 || pins > (8 * sizeof(drv_priv->dout_data))) {
		dev_err(&pdev->dev, "Unsupported number of bits %d", pins);
		return -EINVAL;
	}

	drv_priv = devm_kzalloc(&pdev->dev, struct_size(drv_priv, gpios, pins),
				GFP_KERNEL);
	if (!drv_priv)
		return -ENOMEM;

	for (i = 0; i < pins; i++) {
		struct gpio_desc *out;
		struct gpio_desc *led;

		out = devm_gpiod_get_index(&pdev->dev, "douts", i, GPIOD_OUT_LOW);
		if (IS_ERR(drv_priv->gpios[i].out)) {
			dev_err(&pdev->dev, "Failed to get dout bit %d gpio", i);
			return -EINVAL;
		}
		led = devm_gpiod_get_index_optional(&pdev->dev, "leds", i, GPIOD_OUT_LOW);
		if (IS_ERR(led)) {
			dev_err(&pdev->dev, "Failed to get dout led %d gpio", i);
                	return PTR_ERR(led);
		}

		drv_priv->gpios[i].out = out;
		drv_priv->gpios[i].led = led;
	}

	mutex_init(&drv_priv->lock);

	drv_priv->pin_cnt = pins;

	/* initialize dout with actual gpio value */
	drv_priv->dout_data = 0;
	for (i = 0; i < drv_priv->pin_cnt; i++)
		drv_priv->dout_data = drv_priv->dout_data | (gpiod_get_value(drv_priv->gpios[i].out) << i);

	drv_priv->dout_drv_kobj = kobject_create_and_add("dout_drv", kernel_kobj);
	if (!drv_priv->dout_drv_kobj) {
		dev_err(&pdev->dev, "Cannot create kernel object");
		return -ENOMEM;
	}

	error = sysfs_create_file(drv_priv->dout_drv_kobj, &dout_drv_attribute.attr);
	if (error) {
		dev_err(&pdev->dev, "Cannot create sysfs interface");
		goto release_kobj;
	}

	return 0;


release_kobj:
	kobject_put(drv_priv->dout_drv_kobj);
	return -ENOMEM;
}

static int dout_drv_remove(struct platform_device *pdev)
{
	unsigned int i;

	kobject_put(drv_priv->dout_drv_kobj);

	for (i = 0; i < drv_priv->pin_cnt; i++)	{
		gpiod_set_value(drv_priv->gpios[i].out, 0);
		if (drv_priv->gpios[i].led)
			gpiod_set_value(drv_priv->gpios[i].led, 0);
	}

	dev_info(&pdev->dev, "digital output driver exit\n");
	return 0;
}

static const struct of_device_id of_dout_drv_match[] = {
	{.compatible = "elrest,dout_drv"},
	{ }
};

static struct platform_driver dout_drv = {
	.probe = dout_drv_probe,
	.remove = dout_drv_remove,
	.driver = {
	.name = "dout_drv",
		.owner = THIS_MODULE,
		.of_match_table = of_dout_drv_match,
	}
};

module_platform_driver(dout_drv);

MODULE_AUTHOR("elrest GmbH");
MODULE_DESCRIPTION("dout_drv");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:dout_drv");
