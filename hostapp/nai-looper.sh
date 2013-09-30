#!/bin/sh

# This script can be used to start the host app. It restarts it if it isn't
# running and sends an email notification about the event.

binary="nai"
mailto="nonoo@nonoo.hu"
minmailsendintervalinseconds=600
scriptdir=${0/`basename $0`/}
logfile=$scriptdir/`basename $0`.log

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

lastmailsentat=0
while [ 1 ]; do
	if [ -z "`pidof $binary`" ]; then
		echo "$binary not found, restarting."
		$scriptdir/$binary 2>&1 | awk '{ print strftime("[%Y/%m/%d %H:%M:%S]"), $0; }' >> $binary.log &

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

	$scriptdir/../logrotate/logrotateifneeded.sh $logfile
	$scriptdir/../logrotate/logrotateifneeded.sh $binary.log

	sleep 1
done