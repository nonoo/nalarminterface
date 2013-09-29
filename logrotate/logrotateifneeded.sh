#!/bin/sh

scriptdir=${0/`basename $0`/}
logfile=$scriptdir/logrotate.log

source $scriptdir/config

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

lf=$1

if [ ! -f "$lf" ]; then
	echo "file \"$lf\" doesn't exist."
	exit 1
fi

if [ `du $lf | awk '{print $1}'` -gt $maxlogsizeinkb ]; then
	echo "logfile \"$lf\" is over size, needs rotating."
	$scriptdir/logrotate.sh $lf
fi
