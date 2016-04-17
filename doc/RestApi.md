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

Get Order(s) with optional filter by market and id.

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

Get Trade(s) with optional filter by market and id.

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

Get Posn(s) with optional filter by contract and settlement-date.

View
----

`GET /api/sess/view[/MARKET]`

Get view of the Order-Book with optional filter by market. This is the same as `/api/view` (below),
except that it allows logged-in users to fetch the view along with other session entities in a
single HTTP request.

`GET api/view[/MARKET]`

Get view of the Order-Book with optional filter by market. This is the same as `/api/sess/view`
(above), except that it does not require a login. (The view is considered public information.)
