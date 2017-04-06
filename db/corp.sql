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

INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (1, 'USD', 'United States of America, Dollars', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (2, 'CSCO', 'Cisco Systems Inc', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (3, 'DIS', 'Walt Disney', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (4, 'IBM', 'Ibm Corp', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (5, 'INTC', 'Intel Corp', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (6, 'MSFT', 'Microsoft Corp', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (7, 'VIA', 'Viacom Inc', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (8, 'VOD', 'Vodafone Group Plc', 2)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (9, 'VZ', 'Verizon Com', 2)
;

INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (1, 'CSCO', 'Cisco Systems Inc', 'CSCO', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (2, 'DIS', 'Walt Disney', 'DIS', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (3, 'IBM', 'Ibm Corp', 'IBM', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (4, 'INTC', 'Intel Corp', 'INTC', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (5, 'MSFT', 'Microsoft Corp', 'MSFT', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (6, 'VIA', 'Viacom Inc', 'VIA', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (7, 'VOD', 'Vodafone Group Plc', 'VOD', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (8, 'VZ', 'Verizon Com', 'VZ', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;

COMMIT
;
