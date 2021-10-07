#!/bin/bash

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

rm -f $LAST_ERROR_FILENAME

SDCARD_DEV=$1

test -e ${SDCARD_DEV} || { echo "${SDCARD_DEV} does not exist!" > $LAST_ERROR_FILENAME; exit 1; }

# -j: add journal
# -i: set time intervall before auto-fsck
# - c: set mount count before auto-fsck
#
# Setting them to 0 disables auto-fsck altogether

EXT3_OPTIONS='-j -i 0 -c 0' 

# Abort if device is in use
grep -q $SDCARD_DEV /proc/mounts && { echo "Device $SDCARD_DEV is in use (see mount table)!" > $LAST_ERROR_FILENAME; exit 2; }

sd_card_size=$(fdisk -l ${SDCARD_DEV} | grep MB | awk '{print $5}')

# Delete MBR (win7 stores its FAT there :( )
# --zap-all deletes the MBR

# Create 2 partitions: 1st one 16MB large, 2nd one on the rest of
# space.
if [[ "${sd_card_size}" != "" ]]; then

    echo "create partitions"
    sgdisk --zap-all ${sd_card_device} && \
    sgdisk --resize-table=128 -a 1 \
    -n 1:${FSBL1_FIRST_SECTOR}:${FSBL1_LAST_SECTOR}  -c 1:fsbl1 \
    -n 2:${FSBL2_FIRST_SECTOR}:${FSBL2_LAST_SECTOR}  -c 2:fsbl2 \
    -n 3:${SSBL_FIRST_SECTOR}:${SSBL_LAST_SECTOR} -c 3:ssbl \
    -n 4:${BOOT_FIRST_SECTOR}:${BOOT_LAST_SECTOR} -c 4:BOOT -t 4:0700 \
    -n 5:${ROOTFS_FIRST_SECTOR}: -c 5:rootfs \
    -p ${sd_card_device}
    if [ $? -eq 0 ]; then
      echo "create partitions ok"
    else
      echo "create partitions failed"
      return $INTERNAL_ERROR
    fi

    echo "set legacy BIOS partition"
    sgdisk -A 5:set:2 ${sd_card_device}
    if [ $? -eq 0 ]; then
      echo "set legacy BIOS partition ok"
    else
      echo "set legacy BIOS partition failed"
      return $INTERNAL_ERROR
    fi

    #partprobe "${sd_card_device}"
    update_partition_info "${sd_card_device}" || return "$?"

    # Wait for udev to update the device nodes
    udevadm settle

    # Create file systems.
    print_dbg "create file systems..."
    /sbin/mkfs.vfat "${sd_card_device}p4" -n "BOOT" -i 7761676F

    #do_mkfs
    /sbin/mkfs.ext4 -L "ROOT" -F "${sd_card_device}p5" && \
    /sbin/tune2fs "${EXT4_OPTIONS[@]}" "${sd_card_device}p5"

fi
