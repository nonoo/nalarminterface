#!/ffp/bin/sh

# PROVIDE: nai
# REQUIRE: LOGIN

# This script can be used for starting nai-looper. Place it in /ffp/start or
# edit it and place it in /etc/init.d

. /ffp/etc/ffp.subr

name="nai"
command="/ffp/home/admin/nalarminterface/hostapp/nai-looper.sh"
required_files=""

start_cmd="nai_start"
stop_cmd="nai_stop"

nai_start() {
	echo starting nai
	proc_start_bg $command
}

nai_stop() {
	echo stopping nai
	proc_stop $command
}

run_rc_command "$1"
