#!/bin/sh
MSMTP=/usr/sbin/msmtp
MSMTP_CONF=/etc/msmtprc
MSMTP_CONF_TMP=/tmp/msmtprc_notify.tmp
TMP_NOTIFY_MAIL=/tmp/notify_mail.$$

check_tls() {
	cp $MSMTP_CONF $MSMTP_CONF_TMP
	hostname=`cat $MSMTP_CONF | grep host | awk '{print $2}'`
	check=`$MSMTP -S --host=$hostname --timeout=10 | grep "Support for TLS encryption"`

	if ! [ -z "$check" ]; then
		echo "tls on" >> $MSMTP_CONF_TMP
		echo "tls_certcheck off" >> $MSMTP_CONF_TMP
	fi
}

email_create(){
	sender=`cat $MSMTP_CONF | grep "from" | awk '{print $2}'`
	date=`date`
	dstaddr=$1
	subject=$2
	msg=$3

	echo "Date: $date" > $TMP_NOTIFY_MAIL
	echo "From: $sender" >> $TMP_NOTIFY_MAIL
	echo "To: $dstaddr" >> $TMP_NOTIFY_MAIL
	echo "Content-Type: text/plain;charset=UTF-8" >> $TMP_NOTIFY_MAIL
	echo "Subject: $subject" >> $TMP_NOTIFY_MAIL
	echo "" >> $TMP_NOTIFY_MAIL
	echo -e "$msg" >> $TMP_NOTIFY_MAIL
}

if [ -z "$3" ]; then
	echo "usage: $0 [dstaddr] [subject] [msg] "
	exit 1
fi

DSTADDR=$1
SUBJECT=$2
MSG=$3

check_tls
email_create "${DSTADDR}" "${SUBJECT}" "${MSG}"
cat ${TMP_NOTIFY_MAIL} | ${MSMTP} -C ${MSMTP_CONF_TMP} --timeout=10 -a mailserver1 -t
rm -f ${TMP_NOTIFY_MAIL}
rm -f $MSMTP_CONF_TMP
