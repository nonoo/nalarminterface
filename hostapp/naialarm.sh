#!/bin/sh

self=`readlink "$0"`
if [ -z "$self" ]; then
	self=$0
fi
scriptname=`basename "$self"`
scriptdir=${self%$scriptname}

. $scriptdir/$scriptname-config
. $nlogrotatepath/redirectlog.src.sh

quietmode=1
redirectlog

p1state=$1
p1int=$2
p2state=$3
p2int=$4
p3state=$5
p3int=$6
p4state=$7
p4int=$8

echo "script started with parameters \"$*\"."

if [ ! -z $p1int ] && [ $p1int -eq 1 ]; then
	echo -n "interrupt on p1 - "
	if [ $p1state -eq 1 ]; then
		echo "state on, setting foscam md alarm 1."
#		../foscamctrl/foscam-setmdalarm 1
	else
		echo "state off, setting foscam md alarm 0."
#		../foscamctrl/foscam-setmdalarm 0
	fi
fi

if [ ! -z $p2int ] && [ $p2int -eq 1 ]; then
	echo -n "interrupt on p2 - "
	if [ $p2state -eq 1 ]; then
		echo "state on, sending alarm on sms."
		msg="RIASZTAS, az otthoni riaszto bekapcsolt: `date`"
#		echo $msg | $smsdinject -c $scriptdir/../tc35/smsdrc TEXT $smsto1
#		echo $msg | $smsdinject -c $scriptdir/../tc35/smsdrc TEXT $smsto2

		echo "state on, sending alarm on email."
		subject="[nai] RIASZTAS"
		msg="Az otthoni riaszto bekapcsolt: `date`"
		$scriptdir/../mail/mail.sh nonoo@nonoo.hu "$subject" "$msg"
#		$scriptdir/../mail/mail.sh dianno@dianno.hu "$subject" "$msg"
	else
		echo "state off, doing nothing."
	fi
fi

if [ ! -z $p3int ] && [ $p3int -eq 1 ]; then
	echo -n "interrupt on p3 - "
	if [ $p3state -eq 1 ]; then
		echo "state on, doing nothing."
	else
		echo "state off, doing nothing."
	fi
fi

if [ ! -z $p4int ] && [ $p4int -eq 1 ]; then
	echo -n "interrupt on p4 - "
	if [ $p4state -eq 0 ]; then
		echo "state on, sending sound alarm email."
		subject="[nai] A hang erzekelo bekapcsolt"
		msg="A szenzor hangot erzekelt: `date`"
#		$scriptdir/../mail/mail.sh nonoo@nonoo.hu "$subject" "$msg"
#		$scriptdir/../mail/mail.sh dianno@dianno.hu "$subject" "$msg"
	else
		echo "state off, doing nothing."
	fi
fi

checklogsize

echo "script finished."
