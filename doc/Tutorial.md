Tutorial {#Tutorial}
========

Before you Start
----------------

This tutorial assumes that you have:
- set the `DOOBRY_HOME` environment variable;
- built and installed Doobry;
- created the example database;
- run the `dbrd` daemon.

Configure the Shell
-------------------

Start by setting the following variables in `$HOME/.dbrshrc`:

    # -*- sh -*-
    set user WRAMIREZ
    set group DBRA
    set contr EURUSD
    set settl_date 20140314

Running the Shell
-----------------

Start the shell:

    $ $DOOBRY_HOME/bin/dbrsh 
    dbrsh> 
