//------------------------------------------------------------------------------
/// Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
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
/// \file    din_spi_drv.c
///
/// \brief   Driver for "X3" (8 digital inputs) on EC300 devices
///
/// \author  Ralf Gliese, elrest Automationssysteme GmbH
///
/// The driver expose DIN state via sysfs file din (for example
/// /sys/bus/spi/devices/spi0.0/din).
///
/// Optionally, the driver can control LEDs based on the DIN state. The LED
/// state is refreshed every LED_REFRESH_PERIOD to ensure the LED state is
/// actual even if no user-space process is reading the sysfs file.
///
/// The LED_REFRESH_PERIOD timer is scheduled on each read (this ensures there
/// are not "useless" reads in case the sysfs file is read from user-space
/// more often than LED_REFRESH_PERIOD). The timer expiration schedules work
/// work on work queue which finally read DINs and update LEDs.
///------------------------------------------------------------------------------
//#define DEBUG

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>

#define LED_REFRESH_PERIOD (msecs_to_jiffies(100))

struct din_data {
	unsigned char dma_buffer[2];
	int gpio_load;
	int gpio_tok;
	struct mutex lock;
	unsigned char tx_data;
	unsigned int led_cnt;
	struct din_refresh {
		struct timer_list tim;
		struct workqueue_struct *wq;
		struct work_struct work;
	} led_refresh;
	struct spi_device *spi;
	struct gpio_desc *leds[];
};

/* SPI Write/Read cycle */
static int din_spi_exchange(struct spi_device *spi)
{
	struct din_data *dio = spi_get_drvdata(spi);
	struct spi_message msg;
	struct spi_transfer xfer;
	int ret;

	dio->dma_buffer[0] = 0;
	dio->dma_buffer[1] = 0; //dio->tx_data;

	/* Latch input data */
	gpio_set_value(dio->gpio_load, 0);
	udelay(10);
	gpio_set_value(dio->gpio_load, 1);

	/* Initialize the SPI message and transfer data structures */
	spi_message_init(&msg);
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = xfer.rx_buf = dio->dma_buffer;
	xfer.len = sizeof(dio->dma_buffer);
	spi_message_add_tail(&xfer, &msg);

	/* Send the message and wait for completion */
	ret = spi_sync(spi, &msg);
	if (ret != 0) {
		dev_err(&spi->dev, "SPI transmission failed\n");
		return 0;
	}
	dev_dbg(&spi->dev, "sent %02x %02x, received %02x %02x\n",
			dio->tx_data, dio->tx_data,
			dio->dma_buffer[0], dio->dma_buffer[1]);
	return (dio->dma_buffer[0] | dio->dma_buffer[1] << 8);
}

static int get_din(struct din_data *din)
{
	unsigned int i;
	int ret;

	ret = mutex_lock_interruptible(&din->lock);
	if (ret < 0)
		return ret;

	ret = din_spi_exchange(din->spi);

	/* schedule/re-schedule  LEDs refresh */
	if (din->led_cnt)
		mod_timer(&din->led_refresh.tim, jiffies + LED_REFRESH_PERIOD);

	for (i = 0; i < din->led_cnt; i++) {
		int value = (ret & (1 << i) ? 1 : 0);

		gpiod_set_value(din->leds[i], value);
	}

	mutex_unlock(&din->lock);

	return ret;
}

static ssize_t din_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct spi_device *spi = to_spi_device(dev);
	struct din_data *din = spi_get_drvdata(spi);
	int ret;

	ret = get_din(din);
	if (ret < 0)
		return ret;

	return sprintf(buf, "%d\n", ret);
}

static DEVICE_ATTR(din, S_IRUSR, din_show, NULL);

static void trigger_leds_refresh(struct timer_list *t)
{
	struct din_data *din = from_timer(din, t, led_refresh.tim);

	queue_work(din->led_refresh.wq, &din->led_refresh.work);
}

static void do_leds_refresh(struct work_struct *work)
{
	struct din_data *din = container_of(work, struct din_data, led_refresh.work);

	get_din(din);
}

static int alloc_gpio(struct device *dev, const char *name)
{
	int ret, gpio = of_get_named_gpio(dev->of_node, name, 0);

	if (gpio < 0) {
		dev_err(dev, "GPIO '%s' not found in device tree\n", name);
		return gpio;
	}
	ret = devm_gpio_request_one(dev, gpio, GPIOF_OUT_INIT_HIGH, name);
	if (ret < 0) {
		dev_err(dev, "Cannot request GPIO '%s' %d\n", name, gpio);
		return ret;
	}
	dev_dbg(dev, "GPIO '%s' has Number %d\n", name, gpio);
	return gpio;
}

static int din_spi_drv_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	struct din_data *din;
	unsigned int i;
	int led_cnt;
	int error;

	led_cnt = gpiod_count(dev, "leds");
	if (led_cnt < 0)
		led_cnt = 0;

	din = devm_kzalloc(dev, struct_size(din, leds, led_cnt), GFP_KERNEL);
	if (!din)
		return -ENOMEM;

	din->led_cnt = led_cnt;
	for (i = 0; i < led_cnt; i++) {
		din->leds[i] = devm_gpiod_get_index_optional(dev, "leds", i, GPIOD_OUT_LOW);
		if (IS_ERR(din->leds[i])) {
			dev_err(dev, "Failed to get din led %d gpio", i);
			return PTR_ERR(din->leds[i]);
		}
	}

	din->gpio_load = alloc_gpio(dev, "gpio-load");
	if (din->gpio_load < 0)
		return din->gpio_load;

	din->gpio_tok = alloc_gpio(dev, "gpio-tok");
	if (din->gpio_tok < 0)
		return din->gpio_tok;

	din->spi = spi;
	mutex_init(&din->lock);
	spi_set_drvdata(spi, din);

	/* Initialize and enable output lines */
	din_spi_exchange(spi);
	gpio_set_value(din->gpio_load, 1);

	if (din->led_cnt) {
		din->led_refresh.wq = create_workqueue("din_spi_driver/led_refresh");
		if (!din->led_refresh.wq) {
			dev_err(dev, "cannot create led_refresh work queue(%ld)\n", PTR_ERR(din->led_refresh.wq));
			return PTR_ERR(din->led_refresh.wq);
		}

		INIT_WORK(&din->led_refresh.work, do_leds_refresh);

		timer_setup(&din->led_refresh.tim, trigger_leds_refresh, 0);
		mod_timer(&din->led_refresh.tim, jiffies);
	}

	error = device_create_file(dev, &dev_attr_din);
	if (error < 0)
		dev_err(dev, "cannot create sysfs for din_dpi_drv\n");

	return 0;
}

static int din_spi_drv_remove(struct spi_device *spi)
{
	struct din_data *din = spi_get_drvdata(spi);
	unsigned int i;

	if (din->led_cnt) {
		del_timer_sync(&din->led_refresh.tim);

		flush_workqueue(din->led_refresh.wq);
		destroy_workqueue(din->led_refresh.wq);
	}

	device_remove_file(&spi->dev, &dev_attr_din);

	for (i = 0; i < din->led_cnt; i++)
		gpiod_set_value(din->leds[i], 0);

	return 0;
}

static const struct spi_device_id din_spi_ids[] = {
	{ "din_spi", 0 },
	{ },
};
MODULE_DEVICE_TABLE(spi, din_spi_ids);

static struct spi_driver din_spi_driver = {
	.driver = {
		.name	= "din_spi_driver",
		.owner	= THIS_MODULE,
	},
	.id_table = din_spi_ids,
	.probe	= din_spi_drv_probe,
	.remove = din_spi_drv_remove,
};

module_spi_driver(din_spi_driver);

MODULE_AUTHOR("elrest");
MODULE_DESCRIPTION("din_spi driver");
MODULE_LICENSE("GPL");
