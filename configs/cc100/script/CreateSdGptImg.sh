#!/bin/sh

IMAGESIZE=550 #MB

SECTORSIZE=512 #bytes

FSBL1_FIRST_SECTOR=34
FSBL1_LAST_SECTOR=545

FSBL2_FIRST_SECTOR=546
FSBL2_LAST_SECTOR=1057

SSBL_FIRST_SECTOR=1058
SSBL_LAST_SECTOR=5153

BOOT_FIRST_SECTOR=5154
BOOT_LAST_SECTOR=37921

ROOTFS_FIRST_SECTOR=37922

if [ $# -gt 0 ] ; then
	echo "build sd gpt image"
	bootloader=$1
	imagedir=$2
	fsbl="$imagedir/stm32mp1-tf-a-stm32mp151-cc100.stm32"
	ssbl="$imagedir/barebox-stm32mp151-cc100.img"
	boot_img="$imagedir/boot-mlo.vfat"

	if [ "$bootloader" = "uboot" ]; then
		fsbl="$imagedir/u-boot-spl.stm32"
		ssbl="$imagedir/u-boot.img"
	fi

	echo "$imagedir"
	rm -f "$imagedir/sd.hdimg"

	dd if=/dev/zero of="$imagedir/sd.hdimg" bs=1M count=${IMAGESIZE}
	if [ $? -eq 0 ]; then
		echo "create image ok"
	else
		echo "create image failed"
		exit 1
	fi

	echo "create partitions"
	sgdisk --resize-table=128 -a 1 \
    -n 1:${FSBL1_FIRST_SECTOR}:${FSBL1_LAST_SECTOR}    -c 1:fsbl1   \
    -n 2:${FSBL2_FIRST_SECTOR}:${FSBL2_LAST_SECTOR}  -c 2:fsbl2   \
    -n 3:${SSBL_FIRST_SECTOR}:${SSBL_LAST_SECTOR} -c 3:ssbl    \
    -n 4:${BOOT_FIRST_SECTOR}:${BOOT_LAST_SECTOR} -c 4:BOOT -t 4:0700 \
    -n 5:${ROOTFS_FIRST_SECTOR}:     -c 5:rootfs  \
    -p "$imagedir/sd.hdimg"
	if [ $? -eq 0 ]; then
		echo "create partitions ok"
	else
		echo "create partitions failed"
		exit 2
	fi

	echo "set legacy BIOS partition"
	sgdisk -A 5:set:2 "$imagedir/sd.hdimg"
	if [ $? -eq 0 ]; then
		echo "set legacy BIOS partition ok"
	else
		echo "set legacy BIOS partition failed"
		exit 3
	fi

	echo "check partition layout"
	gdisk -l "$imagedir/sd.hdimg"

	echo "write fsbl1"
    dd if="$fsbl" of="$imagedir/sd.hdimg" seek=${FSBL1_FIRST_SECTOR} bs=${SECTORSIZE} conv=notrunc

	echo "write fsbl2"
	dd if="$fsbl" of="$imagedir/sd.hdimg" seek=${FSBL2_FIRST_SECTOR} bs=${SECTORSIZE} conv=notrunc

    echo "write ssbl"
	dd if="$ssbl" of="$imagedir/sd.hdimg" seek=${SSBL_FIRST_SECTOR} bs=${SECTORSIZE} conv=notrunc

	echo "write BOOT"
	dd if="$boot_img" of="$imagedir/sd.hdimg" seek=${BOOT_FIRST_SECTOR} bs=${SECTORSIZE} conv=notrunc

    echo "write rootfs"
	dd "if=$imagedir/root.ext2" of="$imagedir/sd.hdimg" seek=${ROOTFS_FIRST_SECTOR} bs=${SECTORSIZE} conv=notrunc
    
    echo "check partition layout"
	gdisk -l "$imagedir/sd.hdimg"
else
	echo "please input path containing "
	echo "u-boot.img, u-boot-spl.stm32 and root.tgz"
	echo "generated image sd.hdimg will be stored in the same directory"
	exit 12
fi
