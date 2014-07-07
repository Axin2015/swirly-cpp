module('conversation');

test('fraction to real', function() {
    equal(dbr.fractToReal(100, 1).toFixed(2), "100.00", 'one hundred');
    equal(dbr.fractToReal(1, 100).toFixed(2), "0.01", 'one hundredth');
})

test('price increment', function() {
    var instr = {
        tick_numer: 1,
        tick_denom: 10000,
        price_dp: 4
    };
    equal(dbr.priceInc(instr), "0.0001", 'one ten thousandth');
});

test('quantity increment', function() {
    var instr = {
        lot_numer: 1000000,
        lot_denom: 1,
        lot_dp: 1
    };
    equal(dbr.qtyInc(instr), 1000000, 'one million');
});

module('record');

asyncTest('get all instruments', function() {
    expect(1);
    $.ajax({
        type: 'get',
        url: '/api/instr/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        var v = dict['EURJPY.SPOTFWD'];
        equal(v !== undefined ? v.mnem : undefined, 'EURJPY.SPOTFWD', 'mnem');
    }).always(function() {
        start();
    });
});

asyncTest('get single instrument', function() {
    expect(13);
    $.ajax({
        type: 'get',
        url: '/api/instr/EURJPY.SPOTFWD'
    }).done(function(v) {
        equal(v.mnem, 'EURJPY.SPOTFWD', 'mnem');
        equal(v.display, 'EURJPY.SPOTFWD', 'display');
        equal(v.asset_type, 'CURRENCY', 'asset_type');
        equal(v.instr_type, 'SPOTFWD', 'instr_type');
        equal(v.asset, 'EUR', 'asset');
        equal(v.ccy, 'JPY', 'ccy');
        equal(v.tick_numer, 1, 'tick_numer');
        equal(v.tick_denom, 100, 'tick_denom');
        equal(v.lot_numer, 1000000, 'lot_numer');
        equal(v.lot_denom, 1, 'lot_denom');
        equal(v.price_dp, 2, 'price_dp');
        equal(v.pip_dp, 2, 'pip_dp');
        equal(v.qty_dp, 0, 'qty_dp');
    }).always(function() {
        start();
    });
});

asyncTest('get all markets', function() {
    expect(1);
    $.ajax({
        type: 'get',
        url: '/api/market/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        var v = dict['EURJPY'];
        equal(v !== undefined ? v.mnem : undefined, 'EURJPY', 'mnem');
    }).always(function() {
        start();
    });
});

asyncTest('get single market', function() {
    expect(3);
    $.ajax({
        type: 'get',
        url: '/api/market/EURJPY'
    } ).done(function(v) {
        equal(v.mnem, 'EURJPY', 'mnem');
        equal(v.instr, 'EURJPY.SPOTFWD', 'instr');
        equal(v.tenor, 'SP', 'tenor');
    }).always(function() {
        start();
    });
});

asyncTest('get all traders', function() {
    expect(1);
    $.ajax({
        type: 'get',
        url: '/api/trader/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        var v = dict['WRAMIREZ'];
        equal(v !== undefined ? v.mnem : undefined, 'WRAMIREZ', 'mnem');
    }).always(function() {
        start();
    });
});

asyncTest('get single trader', function() {
    expect(3);
    $.ajax({
        type: 'get',
        url: '/api/trader/WRAMIREZ'
    }).done(function(v) {
        equal(v.mnem, 'WRAMIREZ', 'mnem');
        equal(v.display, 'Wayne Ramirez', 'display');
        equal(v.email, 'wayne.ramirez@doobry.org', 'email');
    }).always(function() {
        start();
    });
});

asyncTest('get all accounts', function() {
    expect(1);
    $.ajax({
        type: 'get',
        url: '/api/accnt/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        var v = dict['DBRA'];
        equal(v !== undefined ? v.mnem : undefined, 'DBRA', 'mnem');
    }).always(function() {
        start();
    });
});

asyncTest('get single account', function() {
    expect(3);
    $.ajax({
        type: 'get',
        url: '/api/accnt/DBRA'
    }).done(function(v) {
        equal(v.mnem, 'DBRA', 'mnem');
        equal(v.display, 'Account A', 'display');
        equal(v.email, 'dbra@doobry.org', 'email');
    }).always(function() {
        start();
    });
});

module('order');

asyncTest('post buy order', function() {
    expect(9);
    $.ajax({
        type: 'post',
        url: '/api/order/WRAMIREZ',
        data: '{"accnt":"DBRA",'
            + ' "market":"EURUSD",'
            + ' "action":"BUY",'
            + ' "ticks":1233,'
            + ' "lots":3}'
    }).done(function(v) {
        v = v.new_order;
        $.ajax({
            type: 'put',
            url: '/api/order/WRAMIREZ/' + v.id,
            data: '{"lots":0}'
        });
        equal(v.status, 'NEW', 'status');
        equal(v.trader, 'WRAMIREZ', 'trader');
        equal(v.accnt, 'DBRA', 'accnt');
        equal(v.market, 'EURUSD', 'market');
        equal(v.action, 'BUY', 'action');
        equal(v.ticks, 1233, 'ticks');
        equal(v.resd, 3, 'resd');
        equal(v.exec, 0, 'exec');
        equal(v.lots, 3, 'lots');
    }).always(function() {
        start();
    });
});

asyncTest('post sell order', function() {
    expect(9);
    $.ajax({
        type: 'post',
        url: '/api/order/WRAMIREZ',
        data: '{"accnt":"DBRA",'
            + ' "market":"EURUSD",'
            + ' "action":"SELL",'
            + ' "ticks":1235,'
            + ' "lots":5}'
    }).done(function(v) {
        v = v.new_order;
        $.ajax({
            type: 'put',
            url: '/api/order/WRAMIREZ/' + v.id,
            data: '{"lots":0}'
        });
        equal(v.status, 'NEW', 'status');
        equal(v.trader, 'WRAMIREZ', 'trader');
        equal(v.accnt, 'DBRA', 'accnt');
        equal(v.market, 'EURUSD', 'market');
        equal(v.action, 'SELL', 'action');
        equal(v.ticks, 1235, 'ticks');
        equal(v.resd, 5, 'resd');
        equal(v.exec, 0, 'exec');
        equal(v.lots, 5, 'lots');
    }).always(function() {
        start();
    });
});
