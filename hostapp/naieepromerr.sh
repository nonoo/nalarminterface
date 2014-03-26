#!/bin/sh

scriptname=`basename $0`
scriptdir=${0%$scriptname}

. $scriptdir/$scriptname-config
. $nlogrotatepath/redirectlog.src.sh

quietmode=1
redirectlog

newpage=$1
newaddr=$2
oldpage=$3
oldaddr=$4

echo "script started with parameters \"$*\"."

echo "sending notify email."
subject="[nai] EEPROM counter increased"
msg="EEPROM counter increased at `date`.\n\nnewpage: $newpage\nnewaddr: $newaddr\noldpage: $oldpage\noldaddr: $oldaddr"
$scriptdir/mail.sh nonoo@nonoo.hu "$subject" "$msg"

checklogsize

echo "script finished."
