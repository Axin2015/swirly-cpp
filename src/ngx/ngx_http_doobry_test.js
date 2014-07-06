var http = require('http');

// Functional.

function each(xs, fn) {
    for (i = 0; i < xs.length; ++i)
        fn(xs[i]);
}

function reach(xs, fn) {
    for (i = xs.length; 0 < i; --i)
        fn(xs[i - 1]);
}

function filter(xs, fn) {
    var ys = [];
    each(xs, function(x) { if (fn(x)) ys.push(x); });
    return ys;
}

function map(fn, xs) {
    var ys = [];
    each(xs, function(x) { ys.push(fn(x)); });
    return ys;
}

function fold(x, ys, fn) {
    each(ys, function(y) { x = fn(x, y); });
    return x;
}

function rfold(xs, y, fn) {
    reach(xs, function(x) { y = fn(x, y); });
    return y;
}

function prod(xs) {
    return fold(1, xs, function(x, y) { return x * y; });
}

function sum(xs) {
    return fold(0, xs, function(x, y) { return x + y; });
}

// Conversions.

function fractToReal(numer, denom) {
    return numer / denom;
};

function realToIncs(real, inc_size) {
    return Math.round(real / inc_size);
};

function incsToReal(incs, inc_size) {
    return incs * inc_size;
};

function qtyToLots(qty, contr) {
    return realToIncs(qty, contr.qty_inc);
};

function lotsToQty(lots, contr) {
    return incsToReal(lots, contr.qty_inc).toFixed(contr.qty_dp);
};

function priceToTicks(price, contr) {
    return realToIncs(price, contr.price_inc);
};

function ticksToPrice(ticks, contr) {
    return incsToReal(ticks, contr.price_inc).toFixed(contr.price_dp);
};

function qtyInc(contr) {
    return fractToReal(contr.lot_numer, contr.lot_denom).toFixed(contr.qty_dp);
};

function priceInc(contr) {
    return fractToReal(contr.tick_numer, contr.tick_denom).toFixed(contr.price_dp);
};

function getRequest(path, fn) {
    var options = {
        method: 'GET',
        hostname: 'localhost',
        port: 8080,
        path: path
    };
    return http.get(options, fn);
}

function postRequest(path, data, fn) {
    var options = {
        method: 'POST',
        hostname: 'localhost',
        port: 8080,
        path: path
    };
    var req = http.request(options, fn);
    req.write(data);
    req.end();
    return req;
}

function Model(trader, giveup) {

    var that = this;

    this.trader = trader;
    this.giveup = giveup;

    this.accnt = undefined;
    this.contr = undefined;
    this.view = undefined;

    this.order = [];
    this.trade = [];
    this.posn = [];

    var getAndSetOrder = function() {
        getRequest('/api/order/' + trader, function(res) {
            var body = '';
            res.on('data', function(chunk) {
                body += chunk;
            });
            res.on('end', function() {
                console.log('order received');
                that.order = JSON.parse(body);
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        });
    }

    var getAndSetTrade = function() {
        getRequest('/api/trade/' + trader, function(res) {
            var body = '';
            res.on('data', function(chunk) {
                body += chunk;
            });
            res.on('end', function() {
                console.log('trade received');
                that.trade = JSON.parse(body);
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        });
    }

    var getAndSetPosn = function() {
        getRequest('/api/posn/' + giveup, function(res) {
            var body = '';
            res.on('data', function(chunk) {
                body += chunk;
            });
            res.on('end', function() {
                console.log('posn received');
                that.posn = JSON.parse(body);
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        });
    }

    var refresh = function() {
        console.log('refresh()');
        getAndSetOrder();
        getAndSetTrade();
        getAndSetPosn();
    }

    var maybeRefresh = function() {
        if (that.accnt === undefined
            || that.contr === undefined
            || that.view === undefined)
            return;
        refresh();
        setInterval(function() {
            refresh();
        }, 2000);
    }

    var getAndSetAccnt = function() {
        getRequest('/api/accnt', function(res) {
            var body = '';
            res.on('data', function(chunk) {
                body += chunk;
            });
            res.on('end', function() {
                console.log('accnt received');
                that.accnt = JSON.parse(body);
                maybeRefresh();
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        });
    };

    var getAndSetContr = function() {
        getRequest('/api/contr', function(res) {
            var body = '';
            res.on('data', function(chunk) {
                body += chunk;
            });
            res.on('end', function() {
                console.log('contr received');
                var dict = [];
                each(JSON.parse(body), function(x) {
                    x.price_inc = priceInc(x);
                    x.qty_inc = qtyInc(x);
                    dict[x.mnem] = x;
                });
                that.contr = dict;
                maybeRefresh();
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        });
    };

    var getAndSetView = function() {
        getRequest('/api/view', function(res) {
            var body = '';
            res.on('data', function(chunk) {
                body += chunk;
            });
            res.on('end', function() {
                console.log('view received');
                that.view = JSON.parse(body);
                maybeRefresh();
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        });
    };

    getRequest('/api/logon', function(res) {
        console.log('logon complete: ' + res.statusCode);
        getAndSetAccnt();
        getAndSetContr();
        getAndSetView();
    }).on('error', function(e) {
        console.log('error: ' + e.message);
    });
}

Model.prototype.placeOrder = function(contr, settl_date, ref, action, price, lots, min_lots) {
    var contr = this.contr[contr];
    var ticks = priceToTicks(price, contr);
    var data = JSON.stringify({
        accnt: this.trader,
        giveup: this.giveup,
        contr: contr.mnem,
        settl_date: settl_date,
        ref: ref,
        action: action,
        ticks: ticks,
        lots: new Number(lots),
        min_lots: new Number(min_lots)
    });
    console.log(data);
}

var model = new Model('WRAMIREZ', 'DBRA');

setInterval(function() {
    model.placeOrder('EURUSD', 20140314, '', 'BUY', 1.2345, 10, 1);
}, 2000);
