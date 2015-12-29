Design {#Design}
======

Please note that, at this time of writing, only the core order-book and matching-engine components
are open-source.

How It Works
------------

Swirly is an electronic trading platform that matches buyers and sellers in a market-place. It is
designed to run either in the Cloud or on dedicated infrastructure.

Market-makers place bids and offers into a central limit order-book. Market-makers can revise or
cancel these "resting" orders. Orders in the order-book are aggregated into price-levels and
presented to market participants as market-data. A trade happens when an order crosses the spread
and matches with an existing bid or offer.

Web Clients
-----------

Swirly can be accessed over the Web using mobile or desktop devices. Once securely authenticated,
traders can access market-data and view their order, trade and position blotters.

Orders placed in a market are matched by price-time priority. Orders that are not matched remain as
working orders in the central order-book until they are either cancelled or filled. Market-data is
disseminated to market-participants with three price-levels and the last-traded price.

Completed orders that are either cancelled or filled are moved to the "done" blotter, where they can
be reviewed and then archived. Similarly, trades can be reviewed in the trade blotter and then
archived.

Elastic Frontend
----------------

Mobile and desktop devices send RESTful read-only operations to the elastic frontend. (REST is a
software architecture for building large-scale, distributed, Web services.) The elastic frontend is
a Web tier that will automatically spawn new frontend instances to cope with increased
demand. Market-data, orders, trades and positions are held in a cache for fast concurrent retrieval
across front-end instances.

Sharded Backend
---------------

Order transactions and other write operations are routed to backend server instances. The backend
can be scaled horizontally by partitioning markets across many backend server instances. New
environments can also be used to create private exchanges that operate in complete isolation from
other environments.

NoSQL Datastore
---------------

Reference and transaction data are stored securely in a NoSQL database that is designed to scale
horizontally across cluster of machines.

Algo Clients
------------

Market Makers and other Algos can use industry-standard FIX connectivity as an alternative to Swirly
Cloud’s RESTful Web API. Our FIX protocol supports standard fields for handling market-data,
execution and drop-copy workflows.
