-- The Restful Matching-Engine.
-- Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

-- Forex.

INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (1, 'CHF', 'Switzerland, Francs', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (2, 'EUR', 'Euro Member Countries, Euro', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (3, 'GBP', 'United Kingdom, Pounds', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (4, 'JPY', 'Japan, Yen', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (5, 'USD', 'United States of America, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (6, 'ZAR', 'South Africa, Rand', 3)
;

-- Coal.

INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (7, 'CAP', 'Central Appalachia Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (8, 'NAP', 'Northern Appalachia Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (9, 'ILB', 'Illinois Basin Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (10, 'PRB', 'Powder River Basin Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (11, 'UIB', 'Uinta Basin Coal', 1)
;

-- Coffee.

INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (12, 'ETB', 'Ethiopia, Birr', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (13, 'WYCA', 'Yirgachefe A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (14, 'WWNA', 'Wenago A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (15, 'WKCA', 'Kochere A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (16, 'WGAA', 'Gelena Abaya A', 1)
;

-- US Corporates.

INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (17, 'CSCO', 'Cisco Systems Inc', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (18, 'DIS', 'Walt Disney', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (19, 'IBM', 'Ibm Corp', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (20, 'INTC', 'Intel Corp', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (21, 'MSFT', 'Microsoft Corp', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (22, 'VIA', 'Viacom Inc', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (23, 'VOD', 'Vodafone Group Plc', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (24, 'VZ', 'Verizon Com', 2)
;

-- Forex.

INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (1, 'EURUSD', 'EURUSD', 'EUR', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (2, 'GBPUSD', 'GBPUSD', 'GBP', 'USD', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (3, 'USDCHF', 'USDCHF', 'USD', 'CHF', 1000000, 1, 1, 10000, 4, 1, 10)
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (4, 'USDJPY', 'USDJPY', 'USD', 'JPY', 1000000, 1, 1, 100, 2, 1, 10)
;

-- Coal.

INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (5, 'CAP', 'Central Appalachia Coal', 'CAP', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (6, 'NAP', 'Northern Appalachia Coal', 'NAP', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (7, 'ILB', 'Illinois Basin Coal', 'ILB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (8, 'PRB', 'Powder River Basin Coal', 'PRB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (9, 'UIB', 'Uinta Basin Coal', 'UIB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;

-- Coffee.

INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (10, 'WYCA', 'Yirgachefe A', 'WYCA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (11, 'WWNA', 'Wenago A', 'WWNA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (12, 'WKCA', 'Kochere A', 'WKCA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (13, 'WGAA', 'Gelena Abaya A', 'WGAA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;

-- US Corporates.

INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (14, 'CSCO', 'Cisco Systems Inc', 'CSCO', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (15, 'DIS', 'Walt Disney', 'DIS', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (16, 'IBM', 'Ibm Corp', 'IBM', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (17, 'INTC', 'Intel Corp', 'INTC', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (18, 'MSFT', 'Microsoft Corp', 'MSFT', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (19, 'VIA', 'Viacom Inc', 'VIA', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (20, 'VOD', 'Vodafone Group Plc', 'VOD', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (21, 'VZ', 'Verizon Com', 'VZ', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;

COMMIT
;
