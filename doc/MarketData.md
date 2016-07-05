Market Data {#MarketData}
===========

Views
-----

Swirly distributes Market-Data in the form of "views". In general, a View is an aggregation of the
Order-Book either by Price or by Quantity:

- Aggregation by Price: each Level represents a summation of all Orders with a specific Price. This
is also known as a "Level 2" Order-Book.

- Aggregation by Quantity: each Level represents the average Price that would be achieved by
sweeping the Order-Book for that Quantity. This is also known as the Volume Weighted Average Price
(VWAP).

It is important to note that these "views" are considered public, because the View is not
client-specific and Market-Maker identities are not revealed.

Electronic Communication Networks (ECNs) in the Forex OTC markets also offer "Screened" Views for
Order-Books that have been Credit-Screened. This means that the Order-Book comprises only those
Orders that have passed a pre-trade credit check. Such views are client-specific and therefore
private.

Swirly supports Unscreened Level 2 Snapshots aggregated by Price. Three levels of Depth are provided
by default, but the system can be recompiled to produce more if need be using the `SWIRLY_DEPTH`
option. The number of Orders that comprise each Level are also conveyed, along with the Last-traded
Time, Price and Quantity.

Subscriptions
-------------

Although the Rest API does not currently support subscriptions, clients can specify a list of
markets they wish to receive using a `GET` request similar to the following:

    GET /view/EURUSD,GBPUSD,USDCHF

Streaming API
-------------

The focus of the current development phase is a clean, correct and efficient Order-Book with a
Restful API. Subsequent phases are likely to focus more on Market-Data distribution over established
Financial APIs, such as FIX, ITCH and Multicast. The priorities of such initiatives will depend
heavily on customer demand. WebSockets or OpenMama APIs are also under consideration.
