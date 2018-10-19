-- The Restful Matching-Engine.
-- Copyright (C) 2013, 2018 Swirly Cloud Limited.
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

INSERT INTO accnt_t (symbol, display, email)
       VALUES ('ADMIN', 'Administrator', 'admin@swirlycloud.com')
;

INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (1, 'AUD', 'Australia, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (2, 'CAD', 'Canada, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (3, 'CHF', 'Switzerland, Francs', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (4, 'CZK', 'Czech Republic, Koruny', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (5, 'DKK', 'Denmark, Kroner', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (6, 'EUR', 'Euro Member Countries, Euro', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (7, 'GBP', 'United Kingdom, Pounds', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (8, 'HKD', 'Hong Kong, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (9, 'HUF', 'Hungary, Forint', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (10, 'ILS', 'Israel, New Shekels', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (11, 'JPY', 'Japan, Yen', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (12, 'MXN', 'Mexico, Pesos', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (13, 'NOK', 'Norway, Krone', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (14, 'NZD', 'New Zealand, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (15, 'PLN', 'Poland, Zlotych', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (16, 'RON', 'Romania, New Lei', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (17, 'SEK', 'Sweden, Kronor', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (18, 'SGD', 'Singapore, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (19, 'THB', 'Thailand, Baht', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (20, 'TRY', 'Turkey, New Lira', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (21, 'USD', 'United States of America, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (22, 'ZAR', 'South Africa, Rand', 3)
;

INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (1, 'AUDUSD', 'AUDUSD', 'AUD', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (2, 'EURCHF', 'EURCHF', 'EUR', 'CHF', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (3, 'EURCZK', 'EURCZK', 'EUR', 'CZK', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (4, 'EURDKK', 'EURDKK', 'EUR', 'DKK', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (5, 'EURGBP', 'EURGBP', 'EUR', 'GBP', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (6, 'EURHUF', 'EURHUF', 'EUR', 'HUF', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (7, 'EURJPY', 'EURJPY', 'EUR', 'JPY', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (8, 'EURNOK', 'EURNOK', 'EUR', 'NOK', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (9, 'EURPLN', 'EURPLN', 'EUR', 'PLN', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (10, 'EURRON', 'EURRON', 'EUR', 'RON', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (11, 'EURSEK', 'EURSEK', 'EUR', 'SEK', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (12, 'EURUSD', 'EURUSD', 'EUR', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (13, 'GBPUSD', 'GBPUSD', 'GBP', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (14, 'NZDUSD', 'NZDUSD', 'NZD', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (15, 'USDCAD', 'USDCAD', 'USD', 'CAD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (16, 'USDCHF', 'USDCHF', 'USD', 'CHF', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (17, 'USDHKD', 'USDHKD', 'USD', 'HKD', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (18, 'USDILS', 'USDILS', 'USD', 'ILS', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (19, 'USDJPY', 'USDJPY', 'USD', 'JPY', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (20, 'USDMXN', 'USDMXN', 'USD', 'MXN', 1000000, 1, 1, 1000, 3, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (21, 'USDSGD', 'USDSGD', 'USD', 'SGD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (22, 'USDTHB', 'USDTHB', 'USD', 'THB', 1000000, 1, 1, 100, 2, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (23, 'USDTRY', 'USDTRY', 'USD', 'TRY', 1000000, 1, 1, 1000, 4, 1, 10)
;
INSERT INTO product_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (24, 'USDZAR', 'USDZAR', 'USD', 'ZAR', 1000000, 1, 1, 1000, 3, 1, 10)
;

-- Example cash market.
INSERT INTO market_t (id, product, settl_day, state)
       VALUES ((SELECT (id << 16) FROM product_t WHERE symbol = 'EURUSD'), 'EURUSD', NULL, 0)
;

COMMIT
;
