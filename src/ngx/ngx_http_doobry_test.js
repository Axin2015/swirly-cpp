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

function Model(trader, giveup) {

    var that = this;

    this.trader = trader;
    this.giveup = giveup;

    this.accnt = undefined;
    this.contr = undefined;

    http.get('http://localhost:8080/api/accnt', function(res) {
        var body = '';
        res.on('data', function(chunk) {
            body += chunk;
        });
        res.on('end', function() {
            that.accnt = JSON.parse(body);
        });
    }).on('error', function(e) {
        console.log('error: ' + e.message);
    });

    http.get('http://localhost:8080/api/contr', function(res) {
        var body = '';
        res.on('data', function(chunk) {
            body += chunk;
        });
        res.on('end', function() {
            that.contr = JSON.parse(body);
        });
    }).on('error', function(e) {
        console.log('error: ' + e.message);
    });

    setInterval(function() {
        that.refresh();
    }, 2000);
}

Model.prototype.refresh = function() {
    console.log('refresh()');
    if (this.accnt === undefined
        || this.contr === undefined)
        return;
    console.log('accnt: ' + JSON.stringify(this.accnt));
    console.log('contr: ' + JSON.stringify(this.contr));
}

var model = new Model('WRAMIREZ', 'DBRA');
