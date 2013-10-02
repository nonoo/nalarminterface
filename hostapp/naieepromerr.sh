#!/bin/sh

scriptname=`basename $0`
scriptdir=${0/$scriptname/}
logfile=$scriptdir/$scriptname.log

source $scriptdir/$scriptname-config
source $scriptdir/../common/redirectlog.src.sh

newpage=$1
newaddr=$2
oldpage=$3
oldaddr=$4

echo "script started with parameters \"$*\"."

echo "sending notify email."
subject="[nai] EEPROM counter increased"
msg="EEPROM counter increased at `date`.\n\nnewpage: $newpage\nnewaddr: $newaddr\noldpage: $oldpage\noldaddr: $oldaddr"
$scriptdir/mail.sh nonoo@nonoo.hu "$subject" "$msg"

$scriptdir/../logrotate/logrotateifneeded.sh $logfile

echo "script finished."
