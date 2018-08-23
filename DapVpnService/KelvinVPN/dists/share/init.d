#!/bin/bash
# Start/stop the DapVpnService daemon.
#
### BEGIN INIT INFO
# Provides:          dapvpnservice
# Required-Start:    $syslog $time
# Required-Stop:     $syslog $time
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: DapVpnService running tool.
# Description:       DapVpnService running tool.
### END INIT INFO


#LSBNAMES='-l'  # Uncomment for LSB name support in /etc/cron.d/

. /lib/lsb/init-functions

#pidfile=/opt/kelvinvpn/run/DapVpnService.pid
binpath=/opt/kelvinvpn/bin/DapVpnService

if [ ! -d "/opt/kelvinvpn/run" ]; then
  mkdir -p /opt/kelvinvpn/run
fi
if [ ! -d "/opt/kelvinvpn/log" ]; then
  mkdir -p /opt/kelvinvpn/log
fi

#test -f $binpath || exit 0

case "$1" in
    start)
	log_begin_msg "Starting DapVpnService..."
	$binpath >>/opt/kelvinvpn/log/dapvpnservice.log 2>/opt/kelvinvpn/log/dapvpnservice.log.err&
	log_end_msg $?
    ;;
    stop)

	log_begin_msg "Stopping DapVpnService..."
	killall DapVpnService
#	PID=`cat $pidfile`
#	kill $PID
	log_end_msg 0
    ;;
     restart|force-reload)
	log_begin_msg "Restarting DapVpnService..."
	service dapvpnservice stop
	service dapvpnservice start
	log_end_msg $?
    ;;
    *)
	log_success_msg "Usage: /etc/init.d/dapvpnservice {start|stop|restart|force-reload}"
	exit 1
esac

exit 0
