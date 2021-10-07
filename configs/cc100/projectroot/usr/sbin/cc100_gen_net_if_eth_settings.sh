#!/bin/bash

case "$1" in 
  ethtool)
    echo -e "up sudo /etc/config-tools/pfc200_ethtool -W -c /etc/eth_settings.conf"
    ;;
  phy-settings)
    echo " pre-up sudo /sbin/ip link set eth0 up && sudo /etc/config-tools/pfc200_ethtool -W -c /etc/eth_settings.conf"
    ;;
  usage-options-names)
    echo
    ;;
  usage-options-description)
    echo
    ;;
esac
