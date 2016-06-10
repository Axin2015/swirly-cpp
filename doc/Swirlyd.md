Swirlyd Manual {#Swirlyd}
==============

Synopsis
--------

Usage: `swirlyd [options]`

Options
-------

### -h ###

Show the help message.

### -n ###

Do not daemonise. I.e. run the process in the foreground.

### -t ###

Run in functional test mode.

Core Config
-----------

### mem_pool ###

Mebibytes (MiB) of memory reserved by memory pool.

### file_mode ###

File creation mode mask. The default is 0027 unless the no-daemon (-n) option is specified.

### run_dir ###

Set the run directory. The default is the root directory ('/') unless the no-daemon (-n) option is
specified.

### log_file ###

Write to log-file. The log-file's parent directory will be created if it does not exist. Relative
paths are interpreted relative to the run directory. The syslog facility is used by default if no
log-file is specified for daemonised processes.

### log_level ###

Set the log-level. Valid range is from 0 to 5 inclusive, where 0=Crit, 1=Error, 2=Warning, 3=Notice,
4=Info and 5=Debug. The default is 4 (Info).

### http_port ###

nHttp port. Defaults to 8080.

### http_user ###

Http user header. The default is "Swirly-User".

### journ_capacity ###

Journal queue capacity. The default is 1024.

Sqlite Config
-------------

### sqlite_journ ###

Sqlite journal database.

### sqlite_model ###

Sqlite model database.

### sqlite_enable_trace ###

Enable SQL tracing.

### sqlite_enable_fkey ###

Enable foreign key constraints.
