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
       VALUES (1, 'ETB', 'Ethiopia, Birr', 3)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (2, 'WYCA', 'Yirgachefe A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (3, 'WWNA', 'Wenago A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (4, 'WKCA', 'Kochere A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (5, 'WGAA', 'Gelena Abaya A', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (6, 'WYCB', 'Yirgachefe B', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (7, 'WWNB', 'Wenago B', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (8, 'WKCB', 'Kochere B', 1)
;
INSERT INTO asset_t (id, symbol, display, type_id)
       VALUES (9, 'WGAB', 'Gelena Abaya B', 1)
;

INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (1, 'WYCA', 'Yirgachefe A', 'WYCA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (2, 'WWNA', 'Wenago A', 'WWNA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (3, 'WKCA', 'Kochere A', 'WKCA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (4, 'WGAA', 'Gelena Abaya A', 'WGAA', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (5, 'WYCB', 'Yirgachefe B', 'WYCB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (6, 'WWNB', 'Wenago B', 'WWNB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (7, 'WKCB', 'Kochere B', 'WKCB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;
INSERT INTO instr_t (id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES (8, 'WGAB', 'Gelena Abaya B', 'WGAB', 'ETB', 1, 1, 1, 1, 0, 1, 10);
;

-- Example cash market.
INSERT INTO market_t (id, instr, settl_day, state)
       VALUES ((SELECT (id << 16) FROM instr_t WHERE symbol = 'WYCA'), 'WYCA', 0, 0)
;

COMMIT
;
