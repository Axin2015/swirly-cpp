(function() {

    // Window in browser.
    var root = this;

    var dbr = Object.create(null);

    dbr.getExpires = function() {
        var expires = new Date();
        expires.setDate(expires.getDate() + 1);
        return expires;
    };
    dbr.setCookie = function(name, value) {
        var value = escape(value) + '; expires=' + dbr.getExpires().toUTCString();
        document.cookie = name + '=' + value;
    };
    // http://www.w3schools.com/js/js_cookies.asp
    dbr.getCookie = function(name) {
        var value = document.cookie;
        var start = value.indexOf(' ' + name + '=');
        if (start == -1) {
            start = value.indexOf(name + '=');
        }
        if (start == -1) {
            value = null;
        } else {
            start = value.indexOf('=', start) + 1;
            var end = value.indexOf(';', start);
            if (end == -1) {
                end = value.length;
            }
            value = unescape(value.substring(start, end));
        }
        return value;
    };
    dbr.setMarketFilter = function(filter) {
        dbr.setCookie('markets', filter.join(','));
    }
    dbr.getMarketFilter = function() {
        var cookie = dbr.getCookie('markets');
        return cookie ? cookie.split(',') : [];
    }
    dbr.fractToReal = function(numer, denom) {
        return numer / denom;
    };
    dbr.priceInc = function(instr) {
        return dbr.fractToReal(instr.tick_numer, instr.tick_denom).toFixed(instr.price_dp);
    };
    dbr.qtyInc = function(instr) {
        return dbr.fractToReal(instr.lot_numer, instr.lot_denom).toFixed(instr.qty_dp);
    };
    dbr.realToIncs = function(real, inc_size) {
        return Math.round(real / inc_size);
    };
    dbr.incsToReal = function(incs, inc_size) {
        return incs * inc_size;
    };
    dbr.qtyToLots = function(qty, instr) {
        return dbr.realToIncs(qty, instr.qty_inc);
    };
    dbr.lotsToQty = function(lots, instr) {
        return dbr.incsToReal(lots, instr.qty_inc).toFixed(instr.qty_dp);
    };
    dbr.priceToTicks = function(price, instr) {
        return dbr.realToIncs(price, instr.price_inc);
    };
    dbr.ticksToPrice = function(ticks, instr) {
        return dbr.incsToReal(ticks, instr.price_inc).toFixed(instr.price_dp);
    };
    dbr.eachKey = function(arr, fn) {
        for (var k in arr) {
            fn(k);
        }
    };
    dbr.eachValue = function(arr, fn) {
        for (var k in arr) {
            fn(arr[k]);
        }
    };
    dbr.eachPair = function(arr, fn) {
        for (var k in arr) {
            fn(k, arr[k]);
        }
    };
    dbr.createRow = function() {
        var tr = document.createElement('tr');
        for (var i = 0; i < arguments.length; ++i) {
            var td = document.createElement('td');
            td.innerText = arguments[i];
            tr.appendChild(td);
        }
        return tr;
    };
    dbr.createAction = function(label, fn) {
        var button = document.createElement('button');
        button.innerText = label;
        button.onclick = fn;
        return button;
    }
    dbr.appendField = function(elem, name, value) {
        var fname = document.createElement('span');
        fname.className = 'dbr-field-name';
        fname.innerText = name;

        var fvalue = document.createElement('span');
        fvalue.className = 'dbr-field-value';
        fvalue.innerText = value;

        elem.appendChild(fname);
        elem.appendChild(fvalue);
        elem.appendChild(document.createElement('br'));
    }
    root.dbr = dbr;
}).call(this);

function Model(trader, accnt, pass, ready) {

    var that = this;

    this.trader = trader;
    this.accnt = accnt;
    this.filter = [];

    this.instrs = undefined;
    this.markets = undefined;
    this.traders = undefined;
    this.accnts = undefined;
    this.orders = undefined;
    this.trades = undefined;
    this.posns = undefined;

    var auth = 'Basic ' + btoa(trader + ':' + pass);
    $.ajaxSetup({
        beforeSend: function(xhr) {
            xhr.setRequestHeader('Authorization', auth);
        }
    });

    var enrich = function() {
        if (that.instrs === undefined
            || that.markets === undefined
            || that.traders === undefined
            || that.accnts === undefined)
            return;

        var filter = dbr.getMarketFilter();
        dbr.eachValue(filter, function(k) {
            if (k in model.markets)
                model.filter[k] = true;
        });

        dbr.eachValue(that.instrs, that.enrichInstr.bind(that));

        dbr.eachValue(that.markets, that.enrichMarket.bind(that));
        $('#market-tbody').replaceWith(that.createMarkets());
        $('#market-tbody button').button({
            icons: {
                primary: 'ui-icon-trash'
            }
        });

        dbr.eachValue(that.traders, that.enrichTrader.bind(that));
        dbr.eachValue(that.accnts, that.enrichAccnt.bind(that));

        ready(that);

        setInterval(function() {
            that.refresh();
        }, 500);
    };

    $.ajax({
        type: 'get',
        url: '/api/instr/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        that.instrs = dict;
        enrich();
    });

    $.ajax({
        type: 'get',
        url: '/api/market/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        that.markets = dict;
        enrich();
    });

    $.ajax({
        type: 'get',
        url: '/api/trader/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        that.traders = dict;
        enrich();
    });

    $.ajax({
        type: 'get',
        url: '/api/accnt/'
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.mnem] = v;
        });
        that.accnts = dict;
        enrich();
    });
}

Model.prototype.subscribe = function(k) {
    if (k in this.markets) {
        this.filter[k] = true;
        dbr.setMarketFilter(Object.keys(this.filter));
        $('#market-tbody').replaceWith(this.createMarkets());
        $('#market-tbody button').button({
            icons: {
                primary: 'ui-icon-trash'
            }
        });
    }
}

Model.prototype.unsubscribe = function(k) {
    if (k in this.filter) {
        delete this.filter[k];
        dbr.setMarketFilter(Object.keys(this.filter));
        $('#market-tbody').replaceWith(this.createMarkets());
        $('#market-tbody button').button({
            icons: {
                primary: 'ui-icon-trash'
            }
        });
    }
}

Model.prototype.enrichInstr = function(v) {
    v.price_inc = dbr.priceInc(v);
    v.qty_inc = dbr.qtyInc(v);
};

Model.prototype.enrichMarket = function(v) {
    v.instr = this.instrs[v.instr];
    v.bid_price = dbr.ticksToPrice(v.bid_ticks, v.instr);
    v.ask_price = dbr.ticksToPrice(v.ask_ticks, v.instr);
};

Model.prototype.enrichTrader = function(v) {
};

Model.prototype.enrichAccnt = function(v) {
};

Model.prototype.enrichOrder = function(v) {
    v.market = this.markets[v.market];
    v.price = dbr.ticksToPrice(v.ticks, v.market.instr);
    v.created = new Date(v.created);
    v.modified = new Date(v.modified);
};

Model.prototype.enrichTrade = function(v) {
    v.market = this.markets[v.market];
    v.price = dbr.ticksToPrice(v.ticks, v.market.instr);
    v.created = new Date(v.created);
    v.modified = new Date(v.modified);
};

Model.prototype.enrichPosn = function(v) {
    v.instr = this.instrs[v.instr];
    if (v.buy_lots > 0) {
        v.buy_ticks = dbr.fractToReal(v.buy_licks, v.buy_lots);
    } else {
        v.buy_ticks = 0;
    }
    if (v.sell_lots > 0) {
        v.sell_ticks = dbr.fractToReal(v.sell_licks, v.sell_lots);
    } else {
        v.sell_ticks = 0;
    }
    v.buy_price = dbr.ticksToPrice(v.buy_ticks, v.instr);
    v.sell_price = dbr.ticksToPrice(v.sell_ticks, v.instr);
};

Model.prototype.createInstrs = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'instr-tbody';
    var ks = Object.keys(this.instrs).sort();
    for (var i = 0; i < ks.length; ++i) {
        var v = this.instrs[ks[i]];
        tbody.appendChild(dbr.createRow(
            v.mnem,
            v.display,
            v.asset_type,
            v.instr_type,
            v.asset,
            v.ccy,
            v.price_inc,
            v.qty_inc,
            v.price_dp,
            v.pip_dp,
            v.qty_dp,
            v.min_lots,
            v.max_lots
        ));
    }
    return tbody;
};

Model.prototype.createMarkets = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'market-tbody';
    var ks = Object.keys(this.markets).sort();
    for (var i = 0; i < ks.length; ++i) {
        var k = ks[i];
        if (!(k in this.filter))
            continue;
        var v = this.markets[k];
        var tr = dbr.createRow(
            v.mnem,
            v.instr.mnem,
            v.tenor,
            v.bid_price + '/' + v.bid_resd,
            v.ask_price + '/' + v.ask_resd
        );
        var td = document.createElement('td');
        var a = dbr.createAction('Unsubscribe', this.unsubscribe.bind(this, v.mnem));
        td.appendChild(a);
        tr.appendChild(td);
        tr.onclick = this.showInstr.bind(this, v.instr);
        tbody.appendChild(tr);
    }
    return tbody;
};

Model.prototype.createTraders = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'trader-tbody';
    var ks = Object.keys(this.traders).sort();
    for (var i = 0; i < ks.length; ++i) {
        var v = this.traders[ks[i]];
        tbody.appendChild(dbr.createRow(
            v.mnem,
            v.display,
            v.email
        ));
    }
    return tbody;
};

Model.prototype.createAccnts = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'accnt-tbody';
    var ks = Object.keys(this.accnts).sort();
    for (var i = 0; i < ks.length; ++i) {
        var v = this.accnts[ks[i]];
        tbody.appendChild(dbr.createRow(
            v.mnem,
            v.display,
            v.email
        ));
    }
    return tbody;
};

Model.prototype.createDepth = function(v) {
    var tbody = document.createElement('tbody');
    tbody.id = 'depth-tbody';
    for (i = 0; i < v.bid_side.count.length; ++i) {
        tbody.appendChild(dbr.createRow(
            v.bid_side.count[i],
            v.bid_side.price[i],
            v.bid_side.resd[i],
            v.ask_side.count[i],
            v.ask_side.price[i],
            v.ask_side.resd[i]
        ));
    }
    return tbody;
};

Model.prototype.createOrders = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'order-tbody';
    var ks = Object.keys(this.orders).sort();
    for (var i = 0; i < ks.length; ++i) {
        var v = this.orders[ks[i]];
        var tr = dbr.createRow(
            v.id,
            v.rev,
            v.status,
            v.trader,
            v.accnt,
            v.ref,
            v.market.mnem,
            v.action,
            v.price,
            v.resd,
            v.exec,
            v.lots,
            v.created,
            v.modified);
        var td = document.createElement('td');
        var a = v.resd > 0
            ? dbr.createAction('Cancel', this.cancelOrder.bind(this, v.id))
            : dbr.createAction('Archive', this.archiveOrder.bind(this, v.id));
        td.appendChild(a);
        tr.appendChild(td);
        tbody.appendChild(tr);
    }
    return tbody;
};

Model.prototype.createTrades = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'trade-tbody';
    var ks = Object.keys(this.trades).sort();
    for (var i = 0; i < ks.length; ++i) {
        var v = this.trades[ks[i]];
        var tr = dbr.createRow(
            v.id,
            v.match,
            v.order,
            v.order_rev,
            v.trader,
            v.accnt,
            v.ref,
            v.market.mnem,
            v.cpty,
            v.role,
            v.action,
            v.price,
            v.resd,
            v.exec,
            v.lots,
            v.settl_date,
            v.created,
            v.modified);
        var td = document.createElement('td');
        var a = dbr.createAction('Archive', this.archiveTrade.bind(this, v.id));
        td.appendChild(a);
        tr.appendChild(td);
        tbody.appendChild(tr);
    }
    return tbody;
};

Model.prototype.createPosns = function() {
    var tbody = document.createElement('tbody');
    tbody.id = 'posn-tbody';
    var ks = Object.keys(this.posns).sort();
    for (var i = 0; i < ks.length; ++i) {
        var v = this.posns[ks[i]];
        tbody.appendChild(dbr.createRow(
            v.accnt,
            v.instr.mnem,
            v.settl_date,
            v.buy_price,
            v.buy_lots,
            v.sell_price,
            v.sell_lots
        ));
    }
    return tbody;
};

Model.prototype.refresh = function() {
    this.refreshMarket();
    this.refreshOrder();
    this.refreshTrade();
    this.refreshPosn();
}

Model.prototype.refreshDepth = function(market) {
    var that = this;
    $.ajax({
        type: 'get',
        url: '/api/depth/' + market
    }).done(function(v) {
        var w = model.markets[v.market];
        v.bid_side.price = new Array(v.bid_side.ticks.length);
        v.ask_side.price = new Array(v.ask_side.ticks.length);
        for (i = 0; i < v.bid_side.count.length; ++i) {
            v.bid_side.price[i] = dbr.ticksToPrice(v.bid_side.ticks[i], w.instr);
            v.ask_side.price[i] = dbr.ticksToPrice(v.ask_side.ticks[i], w.instr);
        }
        $('#depth-tbody').replaceWith(that.createDepth(v));
    });
};

Model.prototype.refreshMarket = function() {
    var that = this;
    $.ajax({
        type: 'get',
        url: '/api/market/'
    }).done(function(arr) {
        $.each(arr, function(k, v) {
            var w = that.markets[v.mnem];
            w.bid_price = dbr.ticksToPrice(v.bid_ticks, w.instr);
            w.bid_resd = v.bid_resd;
            w.ask_price = dbr.ticksToPrice(v.ask_ticks, w.instr);
            w.ask_resd = v.ask_resd;
        });
        $('#market-tbody').replaceWith(that.createMarkets());
        $('#market-tbody button').button({
            icons: {
                primary: 'ui-icon-trash'
            }
        });
    });
};

Model.prototype.refreshOrder = function() {
    var that = this;
    $.ajax({
        type: 'get',
        url: '/api/order/' + that.trader
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict['_' + v.id] = v;
        });
        that.orders = dict;
        dbr.eachValue(that.orders, that.enrichOrder.bind(that));
        $('#order-tbody').replaceWith(that.createOrders())
        $('#order-tbody button').button();
    });
};

Model.prototype.refreshTrade = function() {
    var that = this;
    $.ajax({
        type: 'get',
        url: '/api/trade/' + that.accnt
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict['_' + v.id] = v;
        });
        that.trades = dict;
        dbr.eachValue(that.trades, that.enrichTrade.bind(that));
        $('#trade-tbody').replaceWith(that.createTrades());
        $('#trade-tbody button').button();
    });
};

Model.prototype.refreshPosn = function() {
    var that = this;
    $.ajax({
        type: 'get',
        url: '/api/posn/' + that.accnt
    }).done(function(arr) {
        var dict = [];
        $.each(arr, function(k, v) {
            dict[v.instr] = v;
        });
        that.posns = dict;
        dbr.eachValue(that.posns, that.enrichPosn.bind(that));
        $('#posn-tbody').replaceWith(that.createPosns());
    });
};

Model.prototype.showInstr = function(v) {
    var div = document.createElement('div');
    div.id = 'sidebar';
    div.className = 'ui-dialog-content ui-widget-content ui-corner-all';
    dbr.appendField(div, 'Mnem', v.mnem);
    dbr.appendField(div, 'Display', v.display);
    dbr.appendField(div, 'Asset Type', v.asset_type);
    dbr.appendField(div, 'Instr Type', v.instr_type);
    dbr.appendField(div, 'Asset', v.asset);
    dbr.appendField(div, 'Ccy', v.ccy);
    dbr.appendField(div, 'Price Inc', v.price_inc);
    dbr.appendField(div, 'Qty Inc', v.qty_inc);
    dbr.appendField(div, 'Min Lots', v.min_lots);
    dbr.appendField(div, 'Max Lots', v.max_lots);
    $('#sidebar').replaceWith(div);
};

Model.prototype.submitOrder = function(market, action, price, lots) {
    var that = this;
    var instr = this.markets[market].instr;
    var ticks = dbr.priceToTicks(price, instr);
    $.ajax({
        type: 'post',
        url: '/api/order/' + that.trader,
        data: JSON.stringify({
            accnt: that.accnt,
            market: market,
            action: action,
            ticks: ticks,
            lots: new Number(lots)
        })
    }).done(function(v) {
        var w = v.new_order;
        that.enrichOrder(w);
        that.orders['_' + w.id] = w;
        var w = v.new_posn;
        if (w !== null) {
            that.enrichPosn(w);
            that.posns[w.instr.mnem] = w;
            $('#posn-tbody').replaceWith(that.createPosns());
        }
        for (var i in v.orders) {
            w = v.orders[i];
            that.enrichOrder(w);
            that.orders['_' + w.id] = w;
        }
        $('#order-tbody').replaceWith(that.createOrders())
        $('#order-tbody button').button();
        for (var i in v.trades) {
            w = v.trades[i];
            that.enrichTrade(w);
            that.trades['_' + w.id] = w;
        }
        $('#trade-tbody').replaceWith(that.createTrades());
        $('#trade-tbody button').button();
    }).fail(function(r) {
        var v = $.parseJSON(r.responseText);
        $('#error-num').html(v.num);
        $('#error-msg').html(v.msg);
		$('#error-dialog').dialog('open');
    });
};

Model.prototype.cancelOrder = function(id) {
    var that = this;
    $.ajax({
        type: 'put',
        url: '/api/order/' + that.trader + '/' + id,
        data: '{"lots":0}'
    }).done(function(v) {
        v.market = that.markets[v.market];
        v.price = dbr.ticksToPrice(v.ticks, v.market.instr);
        v.created = new Date(v.created);
        v.modified = new Date(v.modified);
        that.orders['_' + v.id] = v;
        $('#order-tbody').replaceWith(that.createOrders());
        $('#order-tbody button').button();
    }).fail(function(r) {
        var v = $.parseJSON(r.responseText);
        $('#error-num').html(v.num);
        $('#error-msg').html(v.msg);
		$('#error-dialog').dialog('open');
    });
};

Model.prototype.archiveOrder = function(id) {
    var that = this;
    $.ajax({
        type: 'delete',
        url: '/api/order/' + that.trader + '/' + id
    }).done(function(v) {
        delete that.orders['_' + id];
        $('#order-tbody').replaceWith(that.createOrders());
        $('#order-tbody button').button();
    }).fail(function(r) {
        var v = $.parseJSON(r.responseText);
        $('#error-num').html(v.num);
        $('#error-msg').html(v.msg);
		$('#error-dialog').dialog('open');
    });
};

Model.prototype.archiveTrade = function(id) {
    var that = this;
    $.ajax({
        type: 'delete',
        url: '/api/trade/' + that.accnt + '/' + id
    }).done(function(v) {
        delete that.trades['_' + id];
        $('#trade-tbody').replaceWith(that.createTrades());
        $('#trade-tbody button').button();
    }).fail(function(r) {
        var v = $.parseJSON(r.responseText);
        $('#error-num').html(v.num);
        $('#error-msg').html(v.msg);
		$('#error-dialog').dialog('open');
    });

};

var model = null;

// Lifecycle phases.

function documentReady() {

    var sub_market = $('#subscribe-market');

    model = new Model('WRAMIREZ', 'DBRA', 'test', function(model) {
	    sub_market.autocomplete({
		    source: Object.keys(model.markets)
	    });
    });

    $('#subscribe-submit').button()
        .click(function(event) {
            model.subscribe(sub_market.val());
		});

    $('#tabs').tabs();

	$('#new-order-open')
	    .button()
	    .click(function() {
	        $('#new-order-form').dialog('open');
	        $('#new-order-market').autocomplete({
		        source: Object.keys(model.markets)
	        });
		});

	var market = $('#new-order-market'),
	    price = $('#new-order-price'),
	    lots = $('#new-order-lots'),
	    allFields = $([]).add(market).add(price).add(lots);

	$('#new-order-form').dialog({
		autoOpen: false,
		modal: true,
		buttons: {
			Buy: function() {
                model.submitOrder(market.val(), 'BUY', price.val(), lots.val());
	            $(this).dialog('close');
	        },
			Sell: function() {
                model.submitOrder(market.val(), 'SELL', price.val(), lots.val());
	            $(this).dialog('close');
	        }
		},
		close: function() {
			allFields.val('').removeClass('ui-state-error');
		}
	});

	$('#error-dialog').dialog({
		autoOpen: false,
		modal: true,
		buttons: {
			Ok: function() {
	            $(this).dialog('close');
	        }
		}
	});
}
