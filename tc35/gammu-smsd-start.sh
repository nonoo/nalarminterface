#!/ffp/bin/sh

# PROVIDE: gammu-smsd
# REQUIRE: LOGIN

# This script can be used for starting gammu-smsd-looper.
# Place it in /ffp/start or edit it and place it in /etc/init.d

. /ffp/etc/ffp.subr

name="gammu-smsd"
command="/ffp/home/admin/nalarminterface/tc35/gammu-smsd-looper.sh"
required_files=""

start_cmd="gammu_smsd_start"
stop_cmd="gammu_smsd_stop"

gammu_smsd_start() {
	echo loading serial adapter kernel module
	modprobe ch341
	proc_start_bg $command
}

gammu_smsd_stop() {
	proc_stop $command
	killall -9 gammu-smsd-looper.sh
	killall -9 gammu-smsd
	echo unloading serial adapter kernel module
	modprobe -r ch341
}

run_rc_command "$1"
