Admin Guide {#AdminGuide}
===========

Database Creation
-----------------

The example database can be created as follows:

    $ sqlite3 swirly.db < $SWIRLY_HOME/etc/sc_schema.sql
    $ sqlite3 swirly.db < $SWIRLY_HOME/etc/sc_example.sql

Daemon Configuration
--------------------

### Synopsis ###

| option    | description                         |
|-----------|-------------------------------------|
| daemon    | true if server should run as daemon |
| bankfile  | path to bank file                   |
| dbfile    | path to database file               |
| logfile   | path to log file                    |
| journsize | size of journal mempool in bytes    |
| poolsize  | size of main mempool in bytes       |

### Bank ###

### Logger ###

The `logfile` option is ignored unless `daemon=yes`:

| option           | daemon   | foreground |
|------------------|----------|------------|
| logfile=\<path\> | \<path\> | stdout/err |
| logfile=         | syslog   | stdout/err |

### Memory Pool ###

### Example ###

    daemon = yes

    bankfile = swirly.bin
    dbfile = swirly.db
    logfile = swirly.log

    journsize = 1048576
    poolsize = 8388608

Daemon Operations
-----------------

### Starting ###

The `scd` server is typically started as follows:

    $ scd -f swirly.conf

### Log Rotation ###

If the `daemon` and `logfile` options are set, then the daemon process will respond to a `SIGHUP` by
re-opening the logfile. This behaviour can be use to rotate the log, as the following example
demonstrates:

    $ mv swirly.log swirly.log.1
    $ killall -HUP scd
