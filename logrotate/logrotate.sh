#!/bin/sh

scriptname=`basename $0`
scriptdir=${0/$scriptname/}
logfile=$scriptdir/logrotate.log

source $scriptdir/config

logpipe=/tmp/$scriptname.pipe
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

echo "rotating $lf..."
if [ -f $lf.$keepcount ]; then
	echo "  removing $lf.$keepcount"
	rm $lf.$keepcount
fi

i=$((keepcount - 1))
while [ $i -ge 0 ]; do
	if [ -f $lf.$i ]; then
		echo "  moving $lf.$i to $lf.$((i + 1))"
		mv $lf.$i $lf.$((i + 1))
	fi
	i=$((i - 1))
done

echo "  moving $lf to $lf.0"
mv $lf $lf.0
echo "  done."

$scriptdir/../logrotate/logrotateifneeded.sh $logfile
