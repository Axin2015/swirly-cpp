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
       VALUES ('USD', 'United States of America, Dollars', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('CAP', 'Central Appalachia Coal', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('NAP', 'Northern Appalachia Coal', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('ILB', 'Illinois Basin Coal', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('PRB', 'Powder River Basin Coal', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('UIB', 'Uinta Basin Coal', 1)
;

INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('CAP', 'Central Appalachia Coal', 'CAP', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('NAP', 'Northern Appalachia Coal', 'NAP', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('ILB', 'Illinois Basin Coal', 'ILB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('PRB', 'Powder River Basin Coal', 'PRB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('UIB', 'Uinta Basin Coal', 'UIB', 'USD', 1000, 1, 1, 20, 2, 1, 10);
;

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('CAP', 'Central Appalachia Coal', 'CAP', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('NAP', 'Northern Appalachia Coal', 'NAP', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('ILB', 'Illinois Basin Coal', 'ILB', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('PRB', 'Powder River Basin Coal', 'PRB', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('UIB', 'Uinta Basin Coal', 'UIB', NULL, 0)
;

COMMIT
;
