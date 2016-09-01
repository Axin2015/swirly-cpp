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
       VALUES ('ETB', 'Ethiopia, Birr', 3)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WYCA', 'Yirgachefe A', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WWNA', 'Wenago A', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WKCA', 'Kochere A', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WGAA', 'Gelena Abaya A', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WYCB', 'Yirgachefe B', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WWNB', 'Wenago B', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WKCB', 'Kochere B', 1)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('WGAB', 'Gelena Abaya B', 1)
;

INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WYCA', 'Yirgachefe A', 'WYCA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WWNA', 'Wenago A', 'WWNA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WKCA', 'Kochere A', 'WKCA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WGAA', 'Gelena Abaya A', 'WGAA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WYCB', 'Yirgachefe B', 'WYCB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WWNB', 'Wenago B', 'WWNB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WKCB', 'Kochere B', 'WKCB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('WGAB', 'Gelena Abaya B', 'WGAB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('WYCA', 'Yirgachefe A', 'WYCA', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('WWNA', 'Wenago A', 'WWNA', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('WKCA', 'Kochere A', 'WKCA', NULL, 0)
;
INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('WGAA', 'Gelena Abaya A', 'WGAA', NULL, 0)
;

COMMIT
;
