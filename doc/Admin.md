Admin Guide {#Admin}
===========

Basic Usage
-----------

Run the Swirly daemon with default options:

    $ swirlyd

The daemon will log messages to syslog by default:

    $ tail -1 /var/log/system.log
    Feb 21 20:37:24 kernighan.local swirlyd[16876]: starting web server on port 8080

Terminate the daemon process:

    $ killall -TERM swirlyd

Use `/tmp` as the run directory and log messages to a file instead of syslog:

    $ echo 'run_dir=/tmp' >swirlyd.conf
    $ echo 'log_file=/var/log/swirlyd/swirlyd.log' >>swirlyd.conf
    $ swirlyd -f swirlyd.conf
    Feb 21 20:49:38.598 NOTICE  [16916]: opening log file: "/var/log/swirlyd/swirlyd.log"

Log Rotation
------------

The Swirly daemon reopens its log file when it receives a `SIGHUP`:

    $ mv /var/log/swirlyd/swirlyd.log /var/log/swirlyd/swirlyd.log.0
    $ killall -HUP swirlyd
    $ gzip /var/log/swirlyd/swirlyd.log.0

These steps can be automated using the `logrotate` facility if available.
