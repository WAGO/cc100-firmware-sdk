#ifndef _MACH_PFC_STARTUP_H
#define _MACH_PFC_STARTUP_H

/*
 * If needed find here the syntax for cmdline mtd partitions
 *     "mtdparts=omap2-nand.0:4m@0x2940000(kernel1),4m(kernel2)ro,211712k(data), \
 *      512k@0x0(raw_barebox),128k(raw_bareboxenv),512k(raw_product),41088k(raw_recovery)"
 */

#define PFC_EEPROM__I2C_ADDR             0x54
#define PFC_EEPROM__SECTION_OFFSET       0x0
#define PFC_EEPROM__DISPLAY_ROTATION     0x1f8
#define PFC_EEPROM__DISPLAYRES_H         0x1fa
#define PFC_EEPROM__DISPLAYRES_V         0x1fc
#define PFC_EEPROM__LABEL_BASE           0x1fe
#define PFC_EEPROM__LABEL_DEVCONF        (PFC_EEPROM__LABEL_BASE + 0)
#define PFC_EEPROM__LABEL_MACADDR        (PFC_EEPROM__LABEL_BASE + 2)
#define PFC_EEPROM__GPIO_WP              170

struct pac_eeprom_xload_section_hdr {
	u8 boot_mode_id;             /* 0..255 */
	u8 cmdline_len;         /* 1..255 */
};

struct pac_eeprom_xload_section {
	struct pac_eeprom_xload_section_hdr hdr;
	u8 cmdline[256];
};

enum pac_xsection_id {
	PFC_XSECTION_HDR = 1,
	PFC_XSECTION_CMDLINE,
	PFC_XSECTION_ALL,
};

enum pac_eeprom_op {
	PFC_READ,
	PFC_WRITE,
};

#endif /* _MACH_PFC_STARTUP_H */
