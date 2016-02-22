Swirlyd Manual {#Swirlyd}
==============

Synopsis
--------

Usage: `swirlyd [options]`

General Options
---------------

### help ###

Show help message.

### log-level ###

Log level. From 0 to 5, where 0=Crit, 1=Error, 2=Warning, 3=Notice, 4=Info and 5=Debug.

Daemon Options
--------------

### directory ###

Working directory. Defaults to the root directory unless the `no-daemon` option is specified.

### log-file ###

Log file name. Relative paths are relative to the working directory. The argument is optional and
defaults to "/var/log/swirlyd/swirlyd.log". The log file's parent directory will be created if it
does not exist.

The syslog facility is used by default unless either the `log-file` or `no-daemon` option are
specified.

### no-daemon ###

Run process in the foreground. I.e. do not daemonise.

### umask ###

File creation mode mask. Defaults to 0027 unless the `no-daemon` option is specified.

Http Options
------------

### auth-user ###

Http auth-user header. Defaults to "Auth-User".

### http-port ###

Http port. Defaults to 8080.
