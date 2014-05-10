#!/bin/sh

# This script can be used to start the host app. It restarts it if it isn't
# running and sends an email notification about the event.

self=`readlink "$0"`
if [ -z "$self" ]; then
	self=$0
fi
scriptname=`basename "$self"`
scriptdir=${self%$scriptname}

cd $scriptdir

. $scriptdir/$scriptname-config
. $nlogrotatepath/redirectlog.src.sh

quietmode=1
redirectlog

lastmailsentat=0
while [ 1 ]; do
	if [ -z "`pidof $binary`" ]; then
		echo "$binary not found, restarting."
		$scriptdir/$binary 2>&1 | awk '{ print strftime("[%Y/%m/%d %H:%M:%S]"), $0; }' >> $binarylogfile &

		if [ ! -z "$mailto" ]; then
			currdate=`date +%s`
			if [ $((currdate - $lastmailsentat)) -gt $minmailsendintervalinseconds ]; then
				echo "sending notification email."
				subject="[nai] $binary not found, restarting"
				msg="$binary not found, restarting at `date`"
				$scriptdir/../mail/mail.sh "$mailto" "$subject" "$msg"
				lastmailsentat=$currdate
			else
				echo "not sending notification email, because the timeout has not elapsed yet."
			fi
		else
			echo "not sending notification email, because destination mail address has not set in config."
		fi
	fi

	checklogsize
	$nlogrotatepath/logrotateifneeded.sh $binarylogfile logcopytruncate

	sleep 1
done
