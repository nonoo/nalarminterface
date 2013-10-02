# This script redirects stdout to a file and timestamps every line.
# The script should be sourced, not executed directly.

if [ -z "$scriptname" ]; then
	echo "redirectlog error: no scriptname given."
	exit 1
fi

if [ -z "$logfile" ]; then
	echo "redirectlog error: no logfile name given."
	exit 1
fi

# Creating the directory for the logfile if it doesn't exist
mkdir -p `dirname $logfile`

logpipe=/tmp/$scriptname.pipe
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
