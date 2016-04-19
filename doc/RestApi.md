Restful API {#RestApi}
===========

Record
------

`GET /api/rec[/entity[,entity]...]`

Get Record entities, where entity is: asset, contr, market or trader.

### Asset ###

`GET /api/rec/asset[/MNEM]`

Get all Assets or specific Asset with matching mnemonic.

### Contr ###

`GET /api/rec/contr[/MNEM]`

Get all Contracts or specific Contract with matching mnemonic.

### Market ###

`GET /api/rec/market[/MNEM]`

Get all Markets or specific Market with matching mnemonic.

`POST /api/rec/market`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| mnem        | String             | Yes   |
| display     | String             | Yes   |
| contr       | String             | Yes   |
| settlDate   | int (ISO8601)      | No    |
| expiryDate  | int (ISO8601)      | No    |
| state       | int (unused)       | No    |

Create Market. Admin only.

`PUT /api/rec/market/MNEM`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| display     | String             | Yes   |
| state       | int (unused)       | No    |

Update Market. Admin only.

### Trader ###

`GET /api/rec/trader[/MNEM]`

Get all Traders or specific Trader with matching mnemonic.

`POST /api/rec/trader`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| mnem        | String             | Yes   |
| display     | String             | Yes   |
| email       | String             | No    |

Create Trader. If specified, the email address must agree with that of the logged-in user, unless
the logged-in user is an admin.

`PUT /api/rec/trader/MNEM`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| display     | String             | Yes   |
| email       | String             | No    |

Update Trader. If specified, the email address must agree with that of the logged-in user, unless
the logged-in user is an admin.

Session
-------

`GET /api/sess[/entity[,entity]...]`

Get Session entities, where entity is: order, trade, posn or view.

### Order ###

`GET /api/sess/order[/MARKET[/ID]]`

Get Order(s) with optional filter by Market and Id.

`POST /api/sess/order/MARKET`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| side        | 'BUY' or 'SELL'    | Yes   |
| lots        | Long               | Yes   |
| ticks       | Long               | Yes   |
| ref         | String             | No    |
| minLots     | Long               | No    |

Create Order.

`PUT /api/sess/order/MARKET/ID[,ID]...`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| lots        | Long               | Yes   |

Cancel or Revise Order. Revise if lots are greater than zero, otherwise Cancel.

`DELETE /api/sess/order/MARKET/ID[,ID]...`

Archive Order(s).

### Trade ###

`GET /api/sess/trade[/MARKET[/ID]]`

Get Trade(s) with optional filter by Market and Id.

`POST /api/sess/trade/MARKET`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| trader      | String             | Yes   |
| side        | 'BUY' or 'SELL'    | Yes   |
| lots        | Long               | Yes   |
| ticks       | Long               | Yes   |
| ref         | String             | No    |
| role        | 'MAKER' or 'TAKER' | No    |
| cpty        | String             | No    |

Create Trade. Admin only.

`DELETE /api/sess/trade/MARKET/ID[,ID]...`

Archive Trade(s).

### Posn ###

`GET /api/sess/posn[/CONTR[/SETTL_DATE]]`

Get Posn(s) with optional filter by Contract and Settlement-Date.

View
----

`GET /api/view[/MARKET]`

Get view of the Order-Book with optional filter by Market. This is the same as `/api/sess/view`
(below), except that it does not require a login. (The View is considered public information.)

`GET /api/sess/view[/MARKET]`

Get view of the Order-Book with optional filter by Market. This is the same as `/api/view` (above),
except that it allows logged-in users to fetch the View along with other session entities in a
single HTTP request.

In summary, the View aliases serve different use-cases:

- `/api/view` - access to View for anonymous users;
- `/api/sess/view` - alias for authenticated users that allows Order, Trade, Posn and View updates
  to be retrieved in batch.

These aliases can be thought of as hard- or soft-links in a file-system, in that they are just
different ways of addressing the same underlying resource.

Although `/api/sess/view` is private, in the sense that a user must be logged-in to access that URL,
this does not mean that the URL returns a private (or screened) View of the Order-Book.

Future consideration: depending on usage patterns, we may to consider making the anonymous
`/api/view` alias a delayed feed.

See also [market-data](@ref MarketData).
