Tutorial {#Tutorial}
========

Start the Shell
---------------

This tutorial assumes that you have:
- set the `DOOBRY_HOME` environment variable;
- built and installed Doobry;
- created the example database;
- run the `dbrd` daemon.

Set the following variables in `$HOME/.dbrshrc`:

    # -*- sh -*-
    set contr EURUSD
    set giveup DBRA
    set settl_date 20140314
    set trader WRAMIREZ

And then run the shell command:

    $ $DOOBRY_HOME/bin/dbrsh
    dbrsh>

Session
-------

Press `<return>` to log any asynchronous messages:

    dbrsh>
    Jun 19 06:22:28.002 INFO  ready received

The `penv` command prints the configuration:

    dbrsh> penv
    |name      |value   |
    +----------+--------+
    |contr     |EURUSD  |
    |giveup    |DBRA    |
    |settl_date|20140314|
    |trader    |WRAMIREZ|

Logon using the `logon` command:

    dbrsh> logon
    Jun 19 06:25:26.738 INFO  logon received

Order
-----

Use the `buy` command to buy 10 lots:

    dbrsh> buy 1.2344 10
    Jun 19 06:27:51.201 INFO  exec received
    Jun 19 06:27:52.036 INFO  view received

Place a second order at the next price level:

    dbrsh> buy 1.2343 20
    Jun 19 06:29:56.126 INFO  exec received
    Jun 19 06:29:58.001 INFO  view received

Then `sell` to create the opposing side of the market:

    brsh> sell 1.2346 15
    Jun 19 06:32:59.280 INFO  exec received
    Jun 19 06:33:00.000 INFO  view received

Use the `order` command to view your resting orders:

    dbrsh> order
    |id|giveup|contr |settl_date|state|action|price   |lots|resd|exec|last_price|last_lots|
    +--+------+------+----------+-----+------+--------+----+----+----+----------+---------+
    |2 |DBRA  |EURUSD|20140314  |NEW  |BUY   |1.234400|  10|  10|   0|  0.000000|        0|
    |3 |DBRA  |EURUSD|20140314  |NEW  |BUY   |1.234300|  20|  20|   0|  0.000000|        0|
    |4 |DBRA  |EURUSD|20140314  |NEW  |SELL  |1.234600|  15|  15|   0|  0.000000|        0|

Market Data
-----------

The `top` commands shows the top-of-book (best bid and offer) for all liquidity:

    dbrsh> top
    |contr |settl_date|bid_price|bid_lots|bid_count|offer_price|offer_lots|offer_count|
    +------+----------+---------+--------+---------+-----------+----------+-----------+
    |EURUSD|20140314  | 1.234400|      10|        1|   1.234600|        15|          1|

Use the `depth` command to show the depth-of-book for the currently selected contract:

    dbrsh> depth
    |contr |settl_date|level|bid_price|bid_lots|bid_count|offer_price|offer_lots|offer_count|
    +------+----------+-----+---------+--------+---------+-----------+----------+-----------+
    |EURUSD|20140314  |    0| 1.234400|      10|        1|   1.234600|        15|          1|
    |EURUSD|20140314  |    1| 1.234300|      20|        1|          -|         -|          -|
    |EURUSD|20140314  |    2|        -|       -|        -|          -|         -|          -|

Trade
-----

Now cross the spread to create a trade:

    brsh> sell 1.2344 5
    Jun 19 06:39:26.368 INFO  exec received
    Jun 19 06:39:26.368 INFO  exec received
    Jun 19 06:39:26.368 INFO  exec received
    Jun 19 06:39:26.368 INFO  posn received

And view the trades using the `trade` command:

    dbrsh> trade
    |id|order|giveup|contr |settl_date|state|action|price   |lots|resd|exc|last_price|last_lots|role |cpty|
    +--+-----+------+------+----------+-----+------+--------+----+----+---+----------+---------+-----+----+
    |6 |5    |DBRA  |EURUSD|20140314  |TRADE|SELL  |1.234400|   5|   0|  5|  1.234400|        5|TAKER|DBRA|
    |7 |2    |DBRA  |EURUSD|20140314  |TRADE|BUY   |1.234400|  10|   5|  5|  1.234400|        5|MAKER|DBRA|

Note that we are both MAKER and TAKER in this example because we traded with ourselves!

The `order` and `depth` commands reflect the changes to the order-book:

    dbrsh> order
    |id|giveup|contr |settl_date|state|action|price   |lots|resd|exec|last_price|last_lots|
    +--+------+------+----------+-----+------+--------+----+----+----+----------+---------+
    |2 |DBRA  |EURUSD|20140314  |TRADE|BUY   |1.234400|  10|   5|   5|  1.234400|        5|
    |3 |DBRA  |EURUSD|20140314  |NEW  |BUY   |1.234300|  20|  20|   0|  0.000000|        0|
    |4 |DBRA  |EURUSD|20140314  |NEW  |SELL  |1.234600|  15|  15|   0|  0.000000|        0|
    |5 |DBRA  |EURUSD|20140314  |TRADE|SELL  |1.234400|   5|   0|   5|  1.234400|        5|

    dbrsh> depth
    |contr |settl_date|level|bid_price|bid_lots|bid_count|offer_price|offer_lots|offer_count|
    +------+----------+-----+---------+--------+---------+-----------+----------+-----------+
    |EURUSD|20140314  |    0| 1.234400|       5|        1|   1.234600|        15|          1|
    |EURUSD|20140314  |    1| 1.234300|      20|        1|          -|         -|          -|
    |EURUSD|20140314  |    2|        -|       -|        -|          -|         -|          -|

Position
--------

Use the `posn` command to view your positions:

    brsh> posn
    |contr |settl_date|buy_price|buy_lots|sell_price|sell_lots|
    +------+----------+---------+--------+----------+---------+
    |EURUSD|20140314  | 1.234400|       5|  1.234400|        5|
