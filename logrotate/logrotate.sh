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

logfile=$1

if [ ! -f "$logfile" ]; then
	echo "file \"$logfile\" doesn't exist."
	exit 1
fi

echo "rotating $logfile..."
if [ -f $logfile.$keepcount ]; then
	echo "  removing $logfile.$keepcount"
	rm $logfile.$keepcount
fi

i=$((keepcount - 1))
while [ $i -ge 0 ]; do
	if [ -f $logfile.$i ]; then
		echo "  moving $logfile.$i to $logfile.$((i + 1))"
		mv $logfile.$i $logfile.$((i + 1))
	fi
	i=$((i - 1))
done

echo "  moving $logfile to $logfile.0"
mv $logfile $logfile.0
echo "  done."
