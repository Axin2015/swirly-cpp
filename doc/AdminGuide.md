Admin Guide
===========

Configuration
-------------

### Synopsis

| option    | description                         |
|-----------|-------------------------------------|
| daemon    | true if server should run as daemon |
| bankfile  | path to bank file                   |
| dbfile    | path to database file               |
| logfile   | path to log file                    |
| journsize | size of journal mempool in bytes    |
| poolsize  | size of main mempool in bytes       |

### Bank

### Database

### Logger

The `logfile` option is ignored unless `daemon=yes`:

| option         | daemon | foreground |
|----------------|--------|------------|
| logfile=<path> | <path> | stdout/err |
| logfile=       | syslog | stdout/err |

### Memory Pool

### Example

    daemon = yes

    bankfile = doobry.bin
    dbfile = doobry.db
    logfile = doobry.log

    journsize = 1048576
    poolsize = 8388608

Operations
----------

### Starting

The `dbrd` server is typically started as follows:

    $ dbrd -f doobry.conf

### Log Rotation
