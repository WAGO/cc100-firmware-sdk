#!/bin/bash

# If a directory is a mountpoint, its device number is different from that of
# "directory/.."
# If a directory does not exist, it is no mountpoint.
function is_mountpoint
{
    local dir="${1}"

    if [[ -d "${dir}" ]]; then
        dir_device=$(stat "${dir}" | grep -E -o "Device: [0-9a-z/]+")
        dir_point_point_device=$(stat "${dir}/.." | grep -E -o "Device: [0-9a-z/]+")

        [[ "${dir_device}" != "${dir_point_point_device}" ]]
    else
        false
    fi
}

function main
{
    local dest_dir="${1}"
    local source_medium="${2}"

    if [[ ! -d "$dest_dir" ]]; then
        mkdir -p "${dest_dir}"
    fi

    case "${source_medium}" in

        ${INTERNAL_FLASH_NAND})
           ;;

        ${INTERNAL_FLASH_EMMC})

            if is_mountpoint "${dest_dir}"; then
                status="${INVALID_PARAMETER}"
                ReportError "$status" "Error: cannot mount $EMMC_BACKUP_RESTORE_DATA_DEV over $dest_dir because it is a mountpoint."
            else
                mount "$EMMC_BACKUP_RESTORE_DATA_DEV" "${dest_dir}"
            fi

            ;;
        *)
            status=${INVALID_PARAMETER}
            ;;
    esac
}

if [[ "$0" == "${BASH_SOURCE[0]}" ]]; then
    . /etc/config-tools/board_specific_defines
    . "$(dirname "$0")/${BOARD_ID}_commons"
    main "$@"
fi
