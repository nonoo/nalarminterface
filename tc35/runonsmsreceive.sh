#!/bin/sh

# This script gets called when gammu-smsd receives an SMS.
# It then sends the received message to the given email address.

scriptname=`basename $0`
scriptdir=${0/$scriptname/}
logfile=$scriptdir/$scriptname.log

source $scriptdir/$scriptname-config
source $redirectlogscript

quietmode=1
redirectlog

echo "script started with parameters \"$*\"."

for id in "$@"; do
	echo "processing message $id"

	# Format: IN20131004_135052_00_TescoMobile_00.txt
	date=`echo $id | cut -d'_' -f1 | sed -e 's/IN//'`
	time=`echo $id | cut -d'_' -f2`
	from=`echo $id | cut -d'_' -f4`

	subject="[nai] SMS received from $from"
	msg="Received at: ${date:0:4}/${date:4:2}/${date:6:2} ${time:0:2}:${time:2:2}:${time:4:2}\nMail sent at: `date '+%Y/%m/%d %T %Z'`\n\nMessage: `cat $smsinbox/$id`"
	echo "sending mail with subject \"$subject\", message \"$msg\""
	$scriptdir/../mail/mail.sh $mailto "$subject" "$msg"
done

checklogsize

echo "script finished."
