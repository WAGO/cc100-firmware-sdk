#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package wago-custom-install.
#
# Copyright (c) 2015-2018 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   pfcXXX_update_bootloader.sh
#
# Brief:    Update script for PFC bootloaders.
#
# Author:   AGa: WAGO GmbH & Co. KG
# Author:   PEn: WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#


#
# PFCXXXv3 has a central boot partition for MLO bootloader. Barebox 2nd stage bootloader
# is stored in /boot-Directory on ROOT FS.
#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

set -o nounset
set -o errexit

function update_emmc
{
    local bb_path="$1"
    local dev_type="$2"

    local bb_img="${bb_path}.${dev_type}"
    #dd if="${bb_img}" of=/dev/mmcblk1 bs=1k seek=1 skip=1
    dd if="${bb_img}" of=/dev/mmcblk1p1 bs=1k
   
    local loader_mount_point="/tmp/bootloader"
    local update_folder="$loader_mount_point/update"
    mkdir -p "$loader_mount_point"
    mount "/dev/mmcblk1p2" "$loader_mount_point"
    mkdir -p "$update_folder"
    cp "$bb_path"* "$update_folder/"
    mv "$update_folder/"* "$loader_mount_point/"
    sync
    rmdir "$update_folder"
    sync
    umount "$loader_mount_point"
    rmdir  "$loader_mount_point"
    
    return $?
}

function update_emmc_tfa
{
    local tfa_path="$1"
    local dev_type="$2"

    local tfa_img="${tfa_path}.${dev_type}"

    echo 0 > /sys/block/mmcblk1boot0/force_ro
    echo 0 > /sys/block/mmcblk1boot1/force_ro

    dd if="${tfa_img}" of=/dev/mmcblk1boot0
    dd if="${tfa_img}" of=/dev/mmcblk1boot1
   
    sync
   
    echo 1 > /sys/block/mmcblk1boot0/force_ro
    echo 1 > /sys/block/mmcblk1boot1/force_ro
    
    local loader_mount_point="/tmp/bootloader"
    local update_folder="$loader_mount_point/update"
    mkdir -p "$loader_mount_point"
    mount "/dev/mmcblk1p2" "$loader_mount_point"
    mkdir -p "$update_folder"
    cp "$tfa_path"* "$update_folder/"
    mv "$update_folder/"* "$loader_mount_point/"
    sync
    rmdir "$update_folder"
    sync
    umount "$loader_mount_point"
    rmdir  "$loader_mount_point"

    return $?
}

function backup_bootloader
{
    local system_index=$1
    local barebox_compatible_versions="$2"
    local bb_path="$3"
    local tfa_path="$4"
    local dev_type="$5"
    local result=0

    # Determine target ROOT FS device
    local backup_root=""
    local mount_options=""
    if [[ "${dev_type}" == "CC100" ]]; then
        case $system_index in
            "1")
                backup_root="$G_EMMC_ROOT1_DEV"
                ;;
            "2")
                backup_root="$G_EMMC_ROOT2_DEV"
                ;;
            *)
                result=$INTERNAL_ERROR
                ;;
        esac
    else
        ReportError "Error: unknown device (CC100 expected)."
    fi

    # Mount target ROOT FS and copy bootloaders
    local mount_point="/tmp/bootloader-update/backup_root"
    local loader_backup_destination="/boot/loader-backup"
    if [[ "$result" -eq "0" ]]; then
        mkdir -p "$mount_point"
        result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
    fi

    if [[ "$result" -eq "0" ]]; then
        mount $mount_options "$backup_root" "$mount_point"
        result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
        if [[ "$result" -eq "0" ]]; then
            rm -rf "$mount_point$loader_backup_destination"
            mkdir "$mount_point$loader_backup_destination"
            result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            if [[ "$result" -eq "0" ]]; then
                cp "$bb_path"* "$mount_point$loader_backup_destination"
                cp "$tfa_path"* "$mount_point$loader_backup_destination"
                result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            fi
            if [[ "$result" -eq "0" ]]; then
                cp "$barebox_compatible_versions" "$mount_point$loader_backup_destination"
                result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            fi
            sync
            umount "$mount_point"
            rm -rf "/tmp/bootloader-update"
        fi

    fi

    return $result
}

#
# If the installed version is whitelisted, return false, else return true
# If no version is stored in typelabel, the version IS too old
#
function current_bootloader_version_too_old
{
    local too_old="yes"

    local installed_bl_version="$(/etc/config-tools/get_typelabel_value -n BLIDX)"
    installed_bl_version="${installed_bl_version##BLIDX=}" # cut 'BLIDX='

    local whitelist="${BAREBOX_COMPATIBLE_VERSIONS}"

    for version in ${whitelist}; do
        if [[ "${installed_bl_version}" == "${version}" && "${version}" != "99" ]]; then
            too_old="no"
            break
        fi
    done

    if [[ "${too_old}" == "yes" ]]; then
        true
    else
        false
    fi
}

#
# Parameters: $1 - platform type ('pfc100', 'pfc200', 'pfc200v2' ...)
#             $2 - bootloader source path (optional)
#             $3 - system index to store bootloader backup (optional, default: active system, 9: restore backuped bootloader)
#

function __main
{
    local dev_type="${1:-unknown}"
    local base_path="${2:-}"
    local barebox_compatible_versions="${2:-/etc}"
    local system_index=${3:-$(/etc/config-tools/get_systeminfo active-system-number)}

    if [[ -z "$system_index" ]]; then
        ReportError "Error: undefined system index."
        exit $SHELL_ERROR
    fi

    barebox_compatible_versions="${barebox_compatible_versions}/barebox-compatible-versions"
    if [[ -f "$barebox_compatible_versions" ]]; then
        source "$barebox_compatible_versions"
    else
        ReportError "Error: \"$barebox_compatible_versions\" not found."
        exit $SHELL_ERROR
    fi

    if [[ "${dev_type}" == "unknown" ]]; then
        ReportError "Error: unknown device (CC100 expected)."
        exit $SHELL_ERROR
    fi

    # Build source file names
    local bb_base="$(basename ${G_BAREBOX_FILE_BASE})"
    local bb_path=""
    if [[ -n "$base_path" ]]; then
        bb_path="${base_path}/${bb_base}"
    else
        bb_path="${G_BAREBOX_FILE_BASE}"
    fi

    local tfa_base="$(basename ${G_TFA_FILE_BASE})"
    local tfa_path=""
    if [[ -n "$base_path" ]]; then
        tfa_path="${base_path}/${tfa_base}"
    else
        tfa_path="${G_TFA_FILE_BASE}"
    fi

    # Store bootloader backup
    if [[ -z "$system_index" ]]; then
        echo "Note: Skipping bootloader backup in ROOT FS."
    elif [[ "$system_index" -eq "9" ]]; then
        echo "Restore bootloaders from backup at \"$base_path\""
    else
        # Backup bootloader to ROOT FS
        backup_bootloader $system_index "$barebox_compatible_versions" "${bb_path}" "${tfa_path}" "${dev_type}"
        if [[ "$?" -ne "0" ]]; then
            ReportError "Error: Unable to write bootloaders backup to system $system_index"
            exit $INTERNAL_ERROR
        fi
    fi

    # Update bootloader if necessary
    if ! current_bootloader_version_too_old; then
        echo "Note: No bootloader update needed on central place."
        exit $SUCCESS
    fi
    if [[ "${dev_type}" == "CC100" ]]; then
        update_emmc "${bb_path}" "${dev_type}"
        update_emmc_tfa "${tfa_path}" "${dev_type}"
    else
        ReportError "Error: Unknown device ${dev_type}"
    fi

    # Update bootloader version stored in eeprom
    local new_version="${BAREBOX_COMPATIBLE_VERSIONS}"

    # Replace version by 2 if ${BAREBOX_COMPATIBLE_VERSIONS} is empty
    new_version=${new_version:-2}

    echo "Setting bootloader version to ${new_version}..."
    setfwnr ${new_version}

    return $?
}

__main "$@"
