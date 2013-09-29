#/bin/sh

# This script sets all cameras PTZ to the home position.

scriptdir=${0/`basename $0`/}
source $scriptdir/config

getactivesessions() {
	ipport=$1
	user=$2
	pass=$3

	echo -n "getting active sessions on $ipport..."
	res=`curl -s "http://$ipport/cgi-bin/CGIProxy.fcgi?cmd=getSessionList&usr=$user&pwd=$pass"`
	res=`echo $res | grep '<usrCnt>0</usrCnt>'`
	if [ ! -z "$res" ]; then
		echo "no active sessions!"
		return 0
	fi
	return 1
}

getpresetpointname() {
	ipport=$1
	user=$2
	pass=$3
	presetnr=$4

	echo -n "getting PTZ #$presetnr preset point name on $presetnr..."
	PRESETNAME=`curl -s "http://$ipport/cgi-bin/CGIProxy.fcgi?cmd=getPTZPresetPointList&usr=$user&pwd=$pass" | grep "point$presetnr" | sed "s/<.*>\(.*\)<.*>/\1/g" | tr -d ' '`
	if [ -z "$PRESETNAME" ]; then
		echo "error"
		return 1
	fi

	echo "got name: $PRESETNAME"
	return 0
}

gotopresetnr() {
	ipport=$1
	user=$2
	pass=$3
	presetnr=$4

	getpresetpointname $ipport $user $pass $presetnr
	if [ ! $? -eq 0 ]; then
		return 1
	fi

	echo -n "setting PTZ to preset $presetnr ($PRESETNAME) on $IPPORT..."
	res=`curl -s "http://$ipport/cgi-bin/CGIProxy.fcgi?cmd=ptzGotoPresetPoint&name=$PRESETNAME&usr=$user&pwd=$pass" | grep "<result>0</result>"`
	if [ -z "$res" ]; then
		echo "error"
		return 1
	fi

	echo "ok"
	return 0
}

getactivesessions $IPPORT1 $USER1 $PASS1
if [ $? -eq 0 ]; then
	gotopresetnr $IPPORT1 $USER1 $PASS1 $HOMEPRESETNR1
else
	echo "there are active sessions, skipping."
fi

getactivesessions $IPPORT2 $USER2 $PASS2
if [ $? -eq 0 ]; then
	gotopresetnr $IPPORT2 $USER2 $PASS2 $HOMEPRESETNR2
else
	echo "there are active sessions, skipping."
fi
