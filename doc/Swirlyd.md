Swirlyd Manual {#Swirlyd}
==============

Synopsis
--------

Usage: `swirlyd [options]`

Options
-------

### help ###

`-h`

Show the help message.

### directory ###

`-d directory`

Set working directory. The default is the root directory ('/') unless the `no-daemon` option is
specified.

### log-level ###

`-e level`

Set the log-level. Valid range is from 0 to 5 inclusive, where 0=Crit, 1=Error, 2=Warning, 3=Notice,
4=Info and 5=Debug. The default is 4 (Info).

### log-file ###

`-l path`

Write to log-file. Specify '-' for the default log-file path. The default log-file path is
'/var/log/swirlyd/swirlyd.log'. The log-file's parent directory will be created if it does not
exist. Relative paths are interpreted relative to the working directory.

N.B. the syslog facility is used by default when no log-file is specified for daemonised processes.

### mask ###

`-m mask`

File creation mode mask. The default is 0027 unless the `no-daemon` option is specified.

### http-port ###

`-p port`

Http port. Defaults to 8080.

### http-user ###

`-u name`

Http user header. The default is "Swirly-User".

### no-daemon ###

`-n`

Do not daemonise. I.e. run the process in the foreground.

### test ###

`-t`

Run in functional test mode.
