#/bin/sh

# This script sets motion detection alarm on (1) or off (0) on the Foscam cameras.
# Usage: ./foscam-setalarm [1|0]

self=`readlink "$0"`
if [ -z "$self" ]; then
	self=$0
fi
scriptname=`basename "$self"`
scriptdir=${self%$scriptname}

source $scriptdir/config

setalarm() {
	ipport=$1
	user=$2
	pass=$3
	on=$4

	echo -n "setting alarm $on on $ipport..."
	res=`curl -s "http://$ipport/cgi-bin/CGIProxy.fcgi?cmd=setMotionDetectConfig&isEnable=$on&linkage=14&snapInterval=5&sensitivity=1&triggerInterval=5&schedule0=281474976710655&schedule1=281474976710655&schedule2=281474976710655&schedule3=281474976710655&schedule4=281474976710655&schedule5=281474976710655&schedule6=281474976710655&area0=2047&area1=2047&area2=2047&area3=2047&area4=2047&area5=2047&area6=2047&area7=2047&area8=2047&area9=2047&usr=$user&pwd=$pass" | grep "<result>0</result>"`
	if [ ! -z "$res" ]; then
		echo "ok"
		return 0
	fi

	echo "error"
	return 1
}

printusage() {
	echo "usage: $0 [1|0]"
}

ON=$1

if [ -z "$ON" ]; then
	printusage
	exit 1
fi
if [ $ON != 0 ] && [ $ON != 1 ]; then
	printusage
	exit 1
fi
setalarm $IPPORT1 $USER1 $PASS1 $ON
setalarm $IPPORT2 $USER2 $PASS2 $ON

#echo "getting config from $IPPORT1..."
#curl "http://$IPPORT1/cgi-bin/CGIProxy.fcgi?cmd=getMotionDetectConfig&usr=$USER1&pwd=$PASS1"
#echo "getting config from $IPPORT2..."
#curl "http://$IPPORT2/cgi-bin/CGIProxy.fcgi?cmd=getMotionDetectConfig&usr=$USER2&pwd=$PASS2"
