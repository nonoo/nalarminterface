#!/bin/sh

scriptdir=${0/`basename $0`/}
logfile=$scriptdir/`basename $0`.log

newpage=$1
newaddr=$2
oldpage=$3
oldaddr=$4

# This section redirects stdout to a file and timestamps every line.
logpipe=/tmp/`basename $0`.pipe
rm -f $logpipe
# Setting up a trap to delete the pipe on exit
trap "rm -f $logpipe" EXIT
# Creating pipe
mknod $logpipe p
# Reading from the log pipe and processing it.
awk '{ print strftime("[%Y/%m/%d %H:%M:%S]"), $0; }' $logpipe >> $logfile &
# Closing stdout
exec 1>&-
# Redirecting stdout to the pipe
exec 1>$logpipe
exec 2>&1

echo "script started with parameters \"$*\"."

echo "sending notify email."
subject="[nai] EEPROM counter increased"
msg="EEPROM counter increased at `date`.\n\nnewpage: $newpage\nnewaddr: $newaddr\noldpage: $oldpage\noldaddr: $oldaddr"
$scriptdir/mail.sh nonoo@nonoo.hu "$subject" "$msg"

echo "script finished."
