#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

# destinationPartition is not used when codesys backup is done: in this case no
# partition is mounted, instead the current /home which may (or may not) be a
# mount point for the home partition device is overwritten.
destinationPartition=$1

sourceFilename=$2
settingsFilename=${3:-""}

destinationDir=/mnt/restore_target

MOUNTPOINTS=""
UBIATTACH=""

function do_mount
{
  local fstypeArg=${1}
  local dev=${2}
  local mntpoint=${3}

  print_dbg "mounting $dev to $mntpoint"
 
  # Case SD-Card: mtd device is not attached by default
  # ${dev} is "ubi0_0"/"ubi0_1", without "/dev/" prefix!
  if [[ ! -e /dev/${dev} && -n "$fstypeArg" ]]; then
    /usr/sbin/ubiattach -m ${G_UBIFS_MTD_DEV_NR} && UBIATTACH="yes"
  fi
  
  mount ${fstypeArg} ${dev} ${mntpoint} || exit $MOUNT_ERROR 
  # because of 'set -o errexit' in ${BOARD_ID}_commons
  # we only continue if mount succeeds
  MOUNTPOINTS=${mntpoint}:${MOUNTPOINTS}
}

function cleanup
{
  # disable "errexit" to continue cleaning up even after
  # errors

  set +e

  local save_ifs=$IFS
  IFS=':'
  print_dbg "cleaning up mountpoints: $MOUNTPOINTS"
  for mntpoint in ${MOUNTPOINTS}; do
    print_dbg "$mntpoint: "
    umount "${mntpoint}"
    print_dbg $?

    # dirty fix to preserve /boot directory in target partition
    if [[ "$mntpoint" != "${destinationDir}/boot/loader" ]]; then
        rmdir "$mntpoint"
    fi

  done
  IFS=$save_ifs
 
  if [[ "${UBIATTACH}" == "yes" ]]; then
    /usr/sbin/ubidetach -m ${G_UBIFS_MTD_DEV_NR}
  fi

  set -e
}

function system_backup_is_compatible
{
    local status=$EXECUTABLE_ERROR
    local backup_file=$1
    local backup_fw_version
    local current_fw_version
    tar -xz -C /tmp -f ${backup_file} ./etc/REVISIONS
    backup_fw_version=$(cat  /tmp/etc/REVISIONS | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/\...(..)//g')
    current_fw_version=$(cat /etc/REVISIONS     | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/\...(..)//g')
    rm -r /tmp/etc
    print_dbg "[system_backup_is_compatible] backup version : ${backup_fw_version}"
    print_dbg "[system_backup_is_compatible] current version: ${current_fw_version}"
    if [ ${backup_fw_version} = ${current_fw_version} ]; then
      result=$SUCCESS
    fi
    return $result
}

function prepare_partition {
  local target="$1" # "system" | "home"

  if [[ $target == "system" ]]; then
    print_dbg mount partitions...
    if ! system_backup_is_compatible ${sourceFilename}; then
        SetLastError "Backup version doesn't match corrent system version."
        exit $LAST_ERROR_SET
    fi

    mkdir -p $destinationDir
    do_mount "$FSTYPE_ARG" "$destinationPartition" "$destinationDir"

    
    # Be careful! Wiping $destinationPartition!
    # rm .* causes rm to return 1 (because of . and ..). Thus the 'true'
    rm -rf $destinationDir/* $destinationDir/.* 2>&- || true

    #
    # Backup of the system partition contains contents of /boot/loader (mounted boot-partition).
    # Mount boot-partition to $destinationDir/boot/loader to restore 
    #
    mkdir -p ${destinationDir}/boot/loader
    case $flash_type in
        ${INTERNAL_FLASH_NAND})
            #has no boot partition
            ;;
        ${INTERNAL_FLASH_EMMC})
            do_mount "$FSTYPE_ARG" "${G_EMMC_BOOT_DEV}" "${destinationDir}/boot/loader"
            ;;
    esac

  elif [[ $target == "home" ]]; then
    # Be careful! Wiping /home!
    rm -rf /home/*
  fi
}

function extract {
  local sourceFilename="$1"
  local destinationDir="$2"

  if [[ "$(which pv)" != "" ]]; then
    # show progress bar
    totalSize=$(ls -l ${sourceFilename} | awk '{print $5}') # busybox' stat cannot handle -c
    { pv -N $pv_label -tps "$totalSize" -w 70 "${sourceFilename}" | \
    tar 2>/dev/null xzp -C ${destinationDir} -f -; } || exit "$SHELL_ERROR"
  else
    tar 2>/dev/null xzpf "${sourceFilename}" -C ${destinationDir} || exit "$SHELL_ERROR"
  fi
}

function extract_to_home {
  local sourceFilename="$1"
  local destinationDir="/home/"

  if [[ "enabled" == "$(/etc/config-tools/get_runtime_config homedir-on-sdcard)" ]]; then
    # restore home without codesys
    if [[ "$(which pv)" != "" ]]; then
      # show progress bar
      totalSize=$(ls -l "${sourceFilename}" | awk '{print $5}') # busybox' stat cannot handle -c
      { pv -N $pv_label -tps "$totalSize" -w 70 "${sourceFilename}" | \
      tar 2>/dev/null xzp -C ${destinationDir} -f - --exclude ./codesys_root; } || exit "$SHELL_ERROR"
    else
      tar -xf "${sourceFilename}" -C ${destinationDir} --exclude ./codesys_root || exit "$SHELL_ERROR"
    fi

    # restore codesys to /media/sd/
    rm -rf /media/sd/*
    rm -rf /media/sd/.* ||:  # ||: is needed because .* throws error by deleting . and ..
    tar -xvf "${sourceFilename}" -C /media/sd/ ./codesys_root/ --strip-components=2 || exit "$SHELL_ERROR"
    mv -f /media/sd/CODESYSControl_User.cfg /etc/codesys3.d/CODESYSControl_User.cfg 2>/dev/null ||:
  else
    extract "${sourceFilename}" "${destinationDir}"
    mv -f "${destinationDir}/codesys_root/CODESYSControl_User.cfg" /etc/codesys3.d/CODESYSControl_User.cfg 2>/dev/null ||:
  fi
}

function copy_settings {
  local settingsFilename="$1"
  local destinationDir="$2"

  print_dbg copy settings...     
  # The system partition we created  will restore its settings when it boots
  # for the first time. It will look for the settings file in its
  # $DEFAULT_SETTINGS_DIR, so it is copied there. If no settings file is
  # provided by the caller, the current settings have to be backed up and
  # copied over to the new partition.

  mkdir -p ${destinationDir}/${DEFAULT_SETTINGS_DIR}
  if [[ -f "${settingsFilename}" ]]; then
    print_dbg "cp ${settingsFilename} ${destinationDir}/${DEFAULT_SETTINGS_DIR}/${completeSettingsFileName}"
    cp ${settingsFilename} ${destinationDir}/${DEFAULT_SETTINGS_DIR}/${completeSettingsFileName}
  fi

  # Trigger settings restore in auto_firmware_restore script on next bootup
  printf $UPDATE_STATE_COMPLETE > ${destinationDir}/${DEFAULT_SETTINGS_DIR}/${autoRestoreStatusFile}
}

############### main #################

trap cleanup EXIT

pv_label=""

FSTYPE_ARG=''

flash_type=$(/etc/config-tools/get_device_data medium "${destinationPartition}")

case $destinationPartition in
  *ubi*)
    FSTYPE_ARG='-t ubifs';; # mount needs an explicit fstype in this case
esac 


case "$sourceFilename" in
  *${SYSTEM_FILENAME}*)
    pv_label="system"
    prepare_partition "system"
    print_dbg extract rootfs to ${destinationDir}...
    extract "${sourceFilename}" "${destinationDir}"
    copy_settings "${settingsFilename}" "${destinationDir}"
    ;;
  *${CODESYS_FILENAME}*)
    pv_label="codesys"
    if [[ ! "$(/etc/config-tools/get_typelabel_value order)" == 750-83?? ]] \
       && [[ ! "$(/etc/config-tools/get_typelabel_value order)" == 762-34?? ]]; then
      # PFC300 and WP400's codesys restore is handled differently
      prepare_partition "home"
      extract_to_home "${sourceFilename}"
    fi
    ;;
esac

trap - EXIT

cleanup

exit $SUCCESS
