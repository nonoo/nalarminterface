#!/bin/sh

# This script can be used to start gammu-smsd. It restarts it if it isn't
# running and sends an email notification about the event.

scriptname=`basename $0`
scriptdir=${0/$scriptname/}
logfile=$scriptdir/$scriptname.log

source $scriptdir/$scriptname-config
source $redirectlogscript

quietmode=1
redirectlog

mkdir -p $smsdir/error
mkdir -p $smsdir/inbox
mkdir -p $smsdir/outbox
mkdir -p $smsdir/sent

lastmailsentat=0
while [ 1 ]; do
	if [ -z "`pidof $binary`" ]; then
		echo "$binary not found, restarting."
		$binarypath/$binary $binaryparams 2>&1 | awk '{ print strftime("[%Y/%m/%d %H:%M:%S]"), $0; }' >> $binarylogfile &

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
	fi

	checklogsize
	$logrotateifneeded $binarylogfile logcopytruncate

	sleep 1
done
