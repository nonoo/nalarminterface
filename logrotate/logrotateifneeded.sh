#!/bin/sh

scriptname=`basename $0`
scriptdir=${0/$scriptname/}
logfile=$scriptdir/logrotate.log

source $scriptdir/config
source $scriptdir/../hostapp/redirectlog.src.sh

lf=$1

if [ ! -f "$lf" ]; then
	echo "file \"$lf\" doesn't exist."
	exit 1
fi

if [ `du $lf | awk '{print $1}'` -gt $maxlogsizeinkb ]; then
	echo "logfile \"$lf\" is over size, needs rotating."
	$scriptdir/logrotate.sh $lf
fi
