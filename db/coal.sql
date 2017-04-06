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
       VALUES (2, 'CAP', 'Central Appalachia Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (3, 'NAP', 'Northern Appalachia Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (4, 'ILB', 'Illinois Basin Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (5, 'PRB', 'Powder River Basin Coal', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (6, 'UIB', 'Uinta Basin Coal', 1)
;

INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (1, 'CAP', 'Central Appalachia Coal', 'CAP', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (2, 'NAP', 'Northern Appalachia Coal', 'NAP', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (3, 'ILB', 'Illinois Basin Coal', 'ILB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (4, 'PRB', 'Powder River Basin Coal', 'PRB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (5, 'UIB', 'Uinta Basin Coal', 'UIB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;

COMMIT
;
