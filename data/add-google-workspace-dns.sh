#!/bin/bash

resourceGroup="core-infrastructure"
sites=("inductabend.com" "inductabend.com.au" "inductobend.com" "inductobend.com.au" "ibend.com.au" "bends.com.au")
mailExchanges=("aspmx4.googlemail.com" "alt2.aspmx.l.google.com" "aspmx2.googlemail.com" "aspmx3.googlemail.com" "aspmx.l.google.com" "alt1.aspmx.l.google.com" "aspmx5.googlemail.com")
preferences=( 30 20 30 30 10 20 30 )

for site in ${sites[@]}; do
  for i in ${!mailExchanges[@]}; do
    az network dns record-set mx add-record \
      --resource-group ${resourceGroup} \
      --zone-name ${site} \
      --record-set-name "@" \
      --exchange ${mailExchanges[$i]} \
      --preference ${preferences[$i]}
  done
done
