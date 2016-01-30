Rest API {#Rest}
========

Record
------

### Get Record ###

`GET /<front|back>/rec[/type,...]`

Where type is: asset, contr, market or trader.

### Create Market ###

`POST /back/rec/market`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| mnem        | String             | Yes   |
| display     | String             | Yes   |
| contr       | String             | Yes   |
| settlDate   | int (ISO8601)      | No    |
| expiryDate  | int (ISO8601)      | No    |
| state       | int (unused)       | No    |

Admin only.

### Create Trader ###

`POST /back/rec/trader`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| mnem        | String             | Yes   |
| display     | String             | Yes   |
| email       | String             | No    |

If specified, the email address must agree with that of the logged-in user, unless the logged-in
user is an admin.

### Update Market ###

`PUT /back/rec/market`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| mnem        | String             | Yes   |
| display     | String             | Yes   |
| state       | int (unused)       | No    |

### Update Trader ###

`PUT /back/rec/trader`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| mnem        | String             | Yes   |
| display     | String             | Yes   |
| email       | String             | No    |

If specified, the email address must agree with that of the logged-in user, unless the logged-in
user is an admin.

View
----

### Get View ###

`GET <front|back>/view[/MARKET]`

Session
-------

### Get Session ###

`GET /<front|back>/sess[/type[,type]...]`

Where type is: order, trade, posn, quote, or view.

### Get Order ###

`GET /<front|back>/sess/order[/MARKET[/ID]]`

### Get Trade ###

`GET /<front|back>/sess/trade[/MARKET[/ID]]`

### Get Posn ###

`GET /<front|back>/sess/posn[/CONTR[/SETTL_DATE]]`

### Get Quote ###

`GET /<front|back>/sess/quote[/MARKET[/ID]]`

### Get View ###

`GET /<front|back>/sess/view[/MARKET]`

### Archive Order ###

`DELETE /back/sess/order/MARKET/ID[,ID]...`

### Archive Trade ###

`DELETE /back/sess/trade/MARKET/ID[,ID]...`

### Create Order ###

`POST /back/sess/order/MARKET`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| side        | 'BUY' or 'SELL'    | Yes   |
| lots        | Long               | Yes   |
| ticks       | Long               | Yes   |
| ref         | String             | No    |
| minLots     | Long               | No    |

### Create Trade ###

`POST /back/sess/trade/MARKET`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| trader      | String             | Yes   |
| side        | 'BUY' or 'SELL'    | Yes   |
| lots        | Long               | Yes   |
| ticks       | Long               | Yes   |
| ref         | String             | No    |
| role        | 'Maker' or 'Taker' | No    |
| cpty        | String             | No    |

Admin only.

### Create Quote ###

`POST /back/sess/quote/MARKET`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| side        | 'BUY' or 'SELL'    | Yes   |
| lots        | Long               | Yes   |
| ref         | String             | No    |

### Cancel Or Revise Order ###

`PUT /back/sess/order/MARKET/ID[,ID]...`

| Name        | Type               | Req'd |
| ----------- | ------------------ | ----- |
| lots        | Long               | Yes   |

Revise if lots are greater than zero.
Cancel otherwise.
