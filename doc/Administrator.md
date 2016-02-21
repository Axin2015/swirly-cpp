Administration {#Administration}
==============

Getting Started
---------------

Run the Swirly daemon with default options:

    $ swirlyd

The daemon will log messages to syslog by default:

    $ tail -1 /var/log/system.log
    Feb 21 20:37:24 kernighan.local swirlyd[16876]: starting web server on port 8080

Terminate the daemon process:

    $ killall swirlyd

Use `/tmp` as the working directory and log messages to a file instead of syslog:

    $ swirlyd -d/tmp -l
    Feb 21 20:49:38.598 NOTICE  [16916]: opening log file: "/tmp/swirlyd.log"

Log Rotation
------------

The Swirly daemon reopens its log file when it receives a `SIGHUP`:

    $ mv /tmp/swirlyd.log /tmp/swirlyd.log.0
    $ killall swirlyd -HUP
    $ gzip /tmp/swirlyd.log.0
