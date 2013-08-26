--  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
--
--  This file is part of Doobry written by Mark Aylett.
--
--  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
--  General Public License as published by the Free Software Foundation; either version 2 of the
--  License, or (at your option) any later version.
--
--  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
--  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
--  General Public License for more details.
--
--  You should have received a copy of the GNU General Public License along with this program; if
--  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
--  02110-1301 USA.

-- Use ';' on single line to terminate statement.
PRAGMA foreign_keys = ON
;

BEGIN TRANSACTION
;

INSERT INTO asset_type (mnem) VALUES ('COMMODITY')
;
INSERT INTO asset_type (mnem) VALUES ('CORPORATE')
;
INSERT INTO asset_type (mnem) VALUES ('CURRENCY')
;
INSERT INTO asset_type (mnem) VALUES ('EQUITY')
;
INSERT INTO asset_type (mnem) VALUES ('GOVERNMENT')
;
INSERT INTO asset_type (mnem) VALUES ('INDEX')
;

INSERT INTO contr_type (mnem) VALUES ('SPOTFWD')
;
INSERT INTO contr_type (mnem) VALUES ('FUTURE')
;
INSERT INTO contr_type (mnem) VALUES ('OPTION')
;
INSERT INTO contr_type (mnem) VALUES ('SWAP')
;

INSERT INTO asset (mnem, display, type)
        VALUES ('EUR', 'Euro Member Countries, Euro', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('GBP', 'United Kingdom, Pounds', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('AUD', 'Australia, Dollars', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('NZD', 'New Zealand, Dollars', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('USD', 'United States of America, Dollars', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('CAD', 'Canada, Dollars', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('CHF', 'Switzerland, Francs', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('TRY', 'Turkey, New Lira', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('SGD', 'Singapore, Dollars', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('RON', 'Romania, New Lei', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('PLN', 'Poland, Zlotych', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('ILS', 'Israel, New Shekels', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('DKK', 'Denmark, Kroner', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('ZAR', 'South Africa, Rand', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('NOK', 'Norway, Krone', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('SEK', 'Sweden, Kronor', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('HKD', 'Hong Kong, Dollars', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('MXN', 'Mexico, Pesos', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('CZK', 'Czech Republic, Koruny', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('THB', 'Thailand, Baht', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('JPY', 'Japan, Yen', 'CURRENCY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('HUF', 'Hungary, Forint', 'CURRENCY')
;

INSERT INTO asset (mnem, display, type)
        VALUES ('ZC', 'Corn', 'COMMODITY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('ZS', 'Soybeans', 'COMMODITY')
;
INSERT INTO asset (mnem, display, type)
        VALUES ('ZW', 'Wheat', 'COMMODITY')
;

INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURUSD.SPOTFWD', 'EURUSD.SPOTFWD', 'SPOTFWD', 'EUR', 'USD',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('GBPUSD.SPOTFWD', 'GBPUSD.SPOTFWD', 'SPOTFWD', 'GBP', 'USD',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('AUDUSD.SPOTFWD', 'AUDUSD.SPOTFWD', 'SPOTFWD', 'AUD', 'USD',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('NZDUSD.SPOTFWD', 'NZDUSD.SPOTFWD', 'SPOTFWD', 'NZD', 'USD',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDCAD.SPOTFWD', 'USDCAD.SPOTFWD', 'SPOTFWD', 'USD', 'CAD',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDCHF.SPOTFWD', 'USDCHF.SPOTFWD', 'SPOTFWD', 'USD', 'CHF',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDTRY.SPOTFWD', 'USDTRY.SPOTFWD', 'SPOTFWD', 'USD', 'TRY',
        1, 1000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDSGD.SPOTFWD', 'USDSGD.SPOTFWD', 'SPOTFWD', 'USD', 'SGD',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURRON.SPOTFWD', 'EURRON.SPOTFWD', 'SPOTFWD', 'EUR', 'RON',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURPLN.SPOTFWD', 'EURPLN.SPOTFWD', 'SPOTFWD', 'EUR', 'PLN',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDILS.SPOTFWD', 'USDILS.SPOTFWD', 'SPOTFWD', 'USD', 'ILS',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURDKK.SPOTFWD', 'EURDKK.SPOTFWD', 'SPOTFWD', 'EUR', 'DKK',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDZAR.SPOTFWD', 'USDZAR.SPOTFWD', 'SPOTFWD', 'USD', 'ZAR',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURNOK.SPOTFWD', 'EURNOK.SPOTFWD', 'SPOTFWD', 'EUR', 'NOK',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURSEK.SPOTFWD', 'EURSEK.SPOTFWD', 'SPOTFWD', 'EUR', 'SEK',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDHKD.SPOTFWD', 'USDHKD.SPOTFWD', 'SPOTFWD', 'USD', 'HKD',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDMXN.SPOTFWD', 'USDMXN.SPOTFWD', 'SPOTFWD', 'USD', 'MXN',
        1, 1000, 1000000, 1, 3, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURCZK.SPOTFWD', 'EURCZK.SPOTFWD', 'SPOTFWD', 'EUR', 'CZK',
        1, 100, 1000000, 1, 2, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDTHB.SPOTFWD', 'USDTHB.SPOTFWD', 'SPOTFWD', 'USD', 'THB',
        1, 100, 1000000, 1, 2, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('USDJPY.SPOTFWD', 'USDJPY.SPOTFWD', 'SPOTFWD', 'USD', 'JPY',
        1, 100, 1000000, 1, 2, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURHUF.SPOTFWD', 'EURHUF.SPOTFWD', 'SPOTFWD', 'EUR', 'HUF',
        1, 100, 1000000, 1, 2, 1, 10)
;
-- Crosses.
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURGBP.SPOTFWD', 'EURGBP.SPOTFWD', 'SPOTFWD', 'EUR', 'GBP',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURCHF.SPOTFWD', 'EURCHF.SPOTFWD', 'SPOTFWD', 'EUR', 'CHF',
        1, 10000, 1000000, 1, 4, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('EURJPY.SPOTFWD', 'EURJPY.SPOTFWD', 'SPOTFWD', 'EUR', 'JPY',
        1, 100, 1000000, 1, 2, 1, 10)
;

INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('ZC.FUTURE', 'ZC.FUTURE', 'FUTURE', 'ZC', 'USD',
        1, 400, 5000, 1, 2, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('ZS.FUTURE', 'ZS.FUTURE', 'FUTURE', 'ZS', 'USD',
        1, 400, 5000, 1, 2, 1, 10)
;
INSERT INTO contr (mnem, display, type, asset, ccy, tick_numer,
        tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots)
        VALUES ('ZW.FUTURE', 'ZW.FUTURE', 'FUTURE', 'ZW', 'USD',
        1, 400, 5000, 1, 2, 1, 10)
;

INSERT INTO trader (mnem, display, email)
        VALUES ('WRAMIREZ', 'Wayne Ramirez', 'wayne.ramirez@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('SFLORES', 'Steven Flores', 'steven.flores@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('JWRIGHT', 'Juan Wright', 'juan.wright@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('VCAMPBEL', 'Virginia Campbell', 'virginia.campbell@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('GWILSON', 'George Wilson', 'george.wilson@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('BJONES', 'Bobby Jones', 'bobby.jones@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('TLEE', 'Todd Lee', 'todd.lee@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('EEDWARDS', 'Emily Edwards', 'emily.edwards@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('RALEXAND', 'Raymond Alexander', 'raymond.alexander@doobry.org')
;

INSERT INTO trader (mnem, display, email)
        VALUES ('JTHOMAS', 'Joseph Thomas', 'joseph.thomas@doobry.org')
;

INSERT INTO accnt (mnem, display, email)
        VALUES ('DBRA', 'Account A', 'dbra@doobry.org')
;

INSERT INTO accnt (mnem, display, email)
        VALUES ('DBRB', 'Account B', 'dbrb@doobry.org')
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRA'),
        (SELECT id FROM trader WHERE mnem = 'WRAMIREZ'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'WRAMIREZ'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'SFLORES'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'JWRIGHT'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'VCAMPBEL'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'GWILSON'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'BJONES'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'TLEE'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'EEDWARDS'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'RALEXAND'))
;

INSERT INTO memb (accnt, trader)
        VALUES ((SELECT id FROM accnt WHERE mnem = 'DBRB'),
        (SELECT id FROM trader WHERE mnem = 'JTHOMAS'))
;

COMMIT TRANSACTION
;
