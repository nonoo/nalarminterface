#!/bin/sh

# This script gets called when gammu-smsd has failed processing an SMS.
# It sends the message to the given email address.

self=`readlink -f "$0"`
scriptname=`basename "$self"`
scriptdir=${self%$scriptname}

. $scriptdir/$scriptname-config
. $nlogrotatepath/redirectlog.src.sh

quietmode=1
redirectlog

echo "script started with parameters \"$*\"."

for id in "$@"; do
	echo "processing message $id"

	# Format: OUTC20140226_194156_00_1111111111111_sms0.smsbackup
	direction=`echo $id | cut -d'_' -f1 | sed -e 's/[0-9]*//g'`
	date=`echo $id | cut -d'_' -f1 | sed -e "s/$direction//"`
	time=`echo $id | cut -d'_' -f2`
	number=`echo $id | cut -d'_' -f4`
	failtype=`echo $id | cut -d'_' -f5 | cut -d'.' -f1`

	subject="[nai] $failtype failure $number"
	failmsg="`cat $smserrbox/$id`"
	msg="ID: $id\nDirection: $direction\nFailed at: ${date:0:4}/${date:4:2}/${date:6:2} ${time:0:2}:${time:2:2}:${time:4:2}\nMail sent at: `date '+%Y/%m/%d %T %Z'`\n\nMessage:\n\n$failmsg"
	echo "sending mail with subject \"$subject\", message \"$msg\""
	$scriptdir/../mail/mail.sh $mailto "$subject" "$msg"
done

checklogsize

echo "script finished."
