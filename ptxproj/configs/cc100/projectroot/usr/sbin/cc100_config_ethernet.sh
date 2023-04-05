#!/bin/bash

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

# Translate port label to port number

options=

while getopts ":p:i:Ws:" opt; do
  
  case $opt in
    p) options="$options -p $(LabelToPort $OPTARG)";;
    W) options="$options -W";;
    *) options="$options -$opt $OPTARG";;
  esac

done

# NOTE: pfc200_ethtool is tolerant of empty suboptions/multiple commas (sth. like -s ,speed=10M, )

/sbin/ethtool $options
status=$?
sync

exit $status
