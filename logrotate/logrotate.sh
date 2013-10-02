#!/bin/sh

scriptname=`basename $0`
scriptdir=${0/$scriptname/}
logfile=$scriptdir/logrotate.log

source $scriptdir/config
source $scriptdir/../common/redirectlog.src.sh

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
