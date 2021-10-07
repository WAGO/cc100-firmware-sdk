#!/bin/bash

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

codesysConfigBootProjectLocation=$1
currentBootMedium=$2

if [[ "$currentBootMedium" == "$INTERNAL_FLASH_NAND" ]]; then

    case $codesysConfigBootProjectLocation in
      CARD*)
        echo $SD_CARD
        ;;
      HOME*)
        echo $INTERNAL_FLASH_NAND
        ;;
      *)
        echo "unknown"
        ;;
    esac

elif [[ "$currentBootMedium" == "$SD_CARD" ]]; then

    case $codesysConfigBootProjectLocation in
      CARD*)
        echo $SD_CARD
        ;;
      HOME*)
        echo $SD_CARD
        ;;
      *)
        echo "unknown"
        ;;
    esac

fi

