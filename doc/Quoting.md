Quoting {#Quoting}
=======

N.B. Quoting is only applicable to the Java port.

Overview
--------

Quotes are generated in response to quote requests sent by the customer for a specific market and
quantity. Quotes have a Time To Live (TTL), after which they are implicitly cancelled by the
system. A customer may choose to trade on a quote by submitting a "previously quoted" order. These
previously quoted orders are similar to Fill Or Kill (FOK) orders, in that they are either fully
filled or cancelled immediately. Specifically, previously quoted orders cannot be partially filled,
and they are further guaranteed to be filled by exactly one market-maker, which simplifies the
retail settlement process.

Order Cancellation
------------------

An order is said to be "quoted" when some of its residual quantity is referenced by one or more
unexpired quotes. If the market-maker attempts to cancel a quoted order, then the order will be
marked as "pending-cancel" until all of its quotes have expired, at which point, the order will be
cancelled. Although pending-cancel orders are withdrawn from the Central Limit Order-Book (CLOB)
immediately, they may still Execute with existing quotes for up to TTL seconds (until all quotes
have expired). Market-makers are expected to compensate for this in their pricing.

Quoting Model
-------------

Orders have an "available" quantity in addition to the quantities normally associated with
order-driven markets, such as order, residual, executed and last-traded. The available quantity is
used to track the sum of all unexpired quotes associated with an order.

When the matching engine receives a quote request, it searches each side of the order-book in
price-time priority for an order that meets the following requirements:

- has an available quantity greater than or equal to the quote request quantity;
- does not have the pending-cancel flag set.

If such an order exists, then the available quantity is reduced on the order by the quantity
specified on the quote request, and that side of the quote is set to the order's limit-price. A
quote is returned to the customer if such an order is found. Otherwise, a quote response is returned
that indicates a lack of available liquidity.
