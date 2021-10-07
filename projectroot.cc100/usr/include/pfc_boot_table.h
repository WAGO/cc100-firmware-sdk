#ifndef _MACH_PFC_BOOT_TABLE_H
#define _MACH_PFC_BOOT_TABLE_H

#define PFC_BOOT_DEVICE_MASK   0xc0
#define PFC_BOOT_MODE_MASK     0x3f
#define PFC_BOOT_DEVELOP       (1 << 0)
#define PFC_BOOT_UART          (1 << 1)
#define PFC_BOOT_RECOVER       (1 << 2)
#define PFC_BOOT_UPDATE        (1 << 3)
#define PFC_BOOT_OVERWRITE     (1 << 4)
#define PFC_DEVICE_NAND        (1 << 6)
#define PFC_DEVICE_MMC         (1 << 7)
#define PFC_BOOT_MMC_INVALBIT_MASK   (PFC_BOOT_UPDATE | PFC_BOOT_RECOVER)

#define BOOT_TABLE_ENTRY(_id, _name, _cportion_0, _cportion_1, _cportion_2) \
	{ .id = _id, .name = _name, .cportion = { _cportion_0, _cportion_1, _cportion_2, }, }

struct boot_table_entry {
	unsigned char id;
	char *name;
	char *cportion[3];	/* we use three cmdline portions */
};

char pac_cuart[] = "console=ttyO2,115200";
char pac_cnand[] = "rw ubi.mtd=2 rootfstype=ubifs";
char pac_cnand_root1[] = "root=ubi0:ubi_rootfs1";
char pac_cnand_root2[] = "root=ubi0:ubi_rootfs2";
char pac_cmmc[] = "rw root=/dev/mmcblk0p2 rootfstype=ext3 rootwait";

/*
 * boot_id calculation
 * -------------------
 * The boot_id is an 8-bit value consisting of
 * a device_id and a boot_mode_id:
 *
 * boot_id:  [ 7 6 | 5 4 3 2 1 0 ]
 *              |         |
 *         device_id   boot_mode_id
 *
 * device_id:  0: -          boot_mode_id:  [ 5 4 3 2 1 0 ]
 *             1: nand                        x | | | | |
 *             2: mmc                           | | | | develop mode enable bit
 *             3: -                             | | | uart enable bit
 *                                              | | recover mode enable bit
 *                                              | update
 *                                              overwrite mode
 */
struct boot_table_entry pac_boot_table[] = {
	BOOT_TABLE_ENTRY(0x40, NULL, pac_cnand,      NULL, pac_cnand_root1), /* "nand-default-0-0"      0b01.000000 */
	BOOT_TABLE_ENTRY(0x41, NULL, pac_cnand,      NULL, pac_cnand_root1), /* "nand-default-0-1"      0b01.000001 */
	BOOT_TABLE_ENTRY(0x42, NULL, pac_cnand, pac_cuart, pac_cnand_root1), /* "nand-default-1-0"      0b01.000010 */
	BOOT_TABLE_ENTRY(0x43, NULL, pac_cnand, pac_cuart, pac_cnand_root1), /* "nand-default-1-1"      0b01.000011 */
	BOOT_TABLE_ENTRY(0x44, NULL, pac_cnand,      NULL, pac_cnand_root2), /* "nand-recover-0-0"      0b01.000100 */
	BOOT_TABLE_ENTRY(0x45, NULL, pac_cnand,      NULL, pac_cnand_root2), /* "nand-recover-0-1"      0b01.000101 */
	BOOT_TABLE_ENTRY(0x46, NULL, pac_cnand, pac_cuart, pac_cnand_root2), /* "nand-recover-1-0"      0b01.000110 */
	BOOT_TABLE_ENTRY(0x47, NULL, pac_cnand, pac_cuart, pac_cnand_root2), /* "nand-recover-1-1"      0b01.000111 */
	BOOT_TABLE_ENTRY(0x48, NULL, pac_cnand,      NULL, pac_cnand_root1), /* "nand-default-0-0-upd"  0b01.001000 */
	BOOT_TABLE_ENTRY(0x49, NULL, pac_cnand,      NULL, pac_cnand_root1), /* "nand-default-0-1-upd"  0b01.001001 */
	BOOT_TABLE_ENTRY(0x4a, NULL, pac_cnand, pac_cuart, pac_cnand_root1), /* "nand-default-1-0-upd"  0b01.001010 */
	BOOT_TABLE_ENTRY(0x4b, NULL, pac_cnand, pac_cuart, pac_cnand_root1), /* "nand-default-1-1-upd"  0b01.001011 */
	BOOT_TABLE_ENTRY(0x4c, NULL, pac_cnand,      NULL, pac_cnand_root2), /* "nand-recover-0-0-upd"  0b01.001100 */
	BOOT_TABLE_ENTRY(0x4d, NULL, pac_cnand,      NULL, pac_cnand_root2), /* "nand-recover-0-1-upd"  0b01.001101 */
	BOOT_TABLE_ENTRY(0x4e, NULL, pac_cnand, pac_cuart, pac_cnand_root2), /* "nand-recover-1-0-upd"  0b01.001110 */
	BOOT_TABLE_ENTRY(0x4f, NULL, pac_cnand, pac_cuart, pac_cnand_root2), /* "nand-recover-1-1-upd"  0b01.001111 */
	BOOT_TABLE_ENTRY(0x50, NULL,      NULL,      NULL,            NULL), /* "nand-overwrite"        0b01.010000 */
	BOOT_TABLE_ENTRY(0x80, NULL,  pac_cmmc,      NULL,            NULL), /* "mmc-default-0-0"       0b10.000000 */
	BOOT_TABLE_ENTRY(0x81, NULL,  pac_cmmc,      NULL,            NULL), /* "mmc-default-0-1"       0b10.000001 */
	BOOT_TABLE_ENTRY(0x82, NULL,  pac_cmmc, pac_cuart,            NULL), /* "mmc-default-1-0"       0b10.000010 */
	BOOT_TABLE_ENTRY(0x83, NULL,  pac_cmmc, pac_cuart,            NULL), /* "mmc-default-1-1"       0b10.000011 */
	BOOT_TABLE_ENTRY(0x90, NULL,      NULL,      NULL,            NULL), /* "mmc-overwrite"         0b10.010000 */
};

struct boot_table_entry *default_nand_entry = &pac_boot_table[2]; /* 0x42 */
struct boot_table_entry *default_mmc_entry = &pac_boot_table[19]; /* 0x82 */
#define PFC_DEFAULT_BOOT_MODE 0x2 /* xxx-default-1-0: linux console enabled */

#endif /* _MACH_PFC_BOOT_TABLE_H */

