-- The Restful Matching-Engine.
-- Copyright (C) 2013, 2016 Swirly Cloud Limited.
--
-- This program is free software; you can redistribute it and/or modify it under the terms of the
-- GNU General Public License as published by the Free Software Foundation; either version 2 of the
-- License, or (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
-- even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
-- General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along with this program; if
-- not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
-- 02110-1301, USA.

-- Use ';' on single line to terminate statement.
PRAGMA foreign_keys = ON
;

BEGIN TRANSACTION
;

INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('AUD', 'Australia, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('CAD', 'Canada, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('CHF', 'Switzerland, Francs', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('CZK', 'Czech Republic, Koruny', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('DKK', 'Denmark, Kroner', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('EUR', 'Euro Member Countries, Euro', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('GBP', 'United Kingdom, Pounds', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('HKD', 'Hong Kong, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('HUF', 'Hungary, Forint', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('ILS', 'Israel, New Shekels', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('JPY', 'Japan, Yen', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('MXN', 'Mexico, Pesos', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('NOK', 'Norway, Krone', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('NZD', 'New Zealand, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('PLN', 'Poland, Zlotych', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('RON', 'Romania, New Lei', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('SEK', 'Sweden, Kronor', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('SGD', 'Singapore, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('THB', 'Thailand, Baht', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('TRY', 'Turkey, New Lira', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('USD', 'United States of America, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('ZAR', 'South Africa, Rand', 3)
;

INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('AUDUSD', 'AUDUSD', 'AUD', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURCHF', 'EURCHF', 'EUR', 'CHF', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURCZK', 'EURCZK', 'EUR', 'CZK', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURDKK', 'EURDKK', 'EUR', 'DKK', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURGBP', 'EURGBP', 'EUR', 'GBP', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURHUF', 'EURHUF', 'EUR', 'HUF', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURJPY', 'EURJPY', 'EUR', 'JPY', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURNOK', 'EURNOK', 'EUR', 'NOK', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURPLN', 'EURPLN', 'EUR', 'PLN', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURRON', 'EURRON', 'EUR', 'RON', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURSEK', 'EURSEK', 'EUR', 'SEK', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('EURUSD', 'EURUSD', 'EUR', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('GBPUSD', 'GBPUSD', 'GBP', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('NZDUSD', 'NZDUSD', 'NZD', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDCAD', 'USDCAD', 'USD', 'CAD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDCHF', 'USDCHF', 'USD', 'CHF', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDHKD', 'USDHKD', 'USD', 'HKD', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDILS', 'USDILS', 'USD', 'ILS', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDJPY', 'USDJPY', 'USD', 'JPY', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDMXN', 'USDMXN', 'USD', 'MXN', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDSGD', 'USDSGD', 'USD', 'SGD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDTHB', 'USDTHB', 'USD', 'THB', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDTRY', 'USDTRY', 'USD', 'TRY', 1000000, 1, 1, 1000, 4, 1, 10)
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('USDZAR', 'USDZAR', 'USD', 'ZAR', 1000000, 1, 1, 1000, 3, 1, 10)
;

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('EURUSD', 'EURUSD', 'EURUSD', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('GBPUSD', 'GBPUSD', 'GBPUSD', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('USDCHF', 'USDCHF', 'USDCHF', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('USDJPY', 'USDJPY', 'USDJPY', NULL, 0)
;

COMMIT
;
