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
       VALUES ('CSCO', 'Cisco Systems Inc', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('DIS', 'Walt Disney', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('IBM', 'Ibm Corp', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('INTC', 'Intel Corp', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('MSFT', 'Microsoft Corp', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('VIA', 'Viacom Inc', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('VOD', 'Vodafone Group Plc', 2)
;
INSERT INTO asset_t (mnem, display, type_id)
       VALUES ('VZ', 'Verizon Com', 2)
;

INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('CSCO', 'Cisco Systems Inc', 'CSCO', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('DIS', 'Walt Disney', 'DIS', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('IBM', 'Ibm Corp', 'IBM', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('INTC', 'Intel Corp', 'INTC', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('MSFT', 'Microsoft Corp', 'MSFT', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('VIA', 'Viacom Inc', 'VIA', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('VOD', 'Vodafone Group Plc', 'VOD', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;
INSERT INTO contr_t (mnem, display, asset, ccy, lot_numer, lot_denom,
       tick_numer, tick_denom, pip_dp, min_lots, max_lots)
       VALUES ('VZ', 'Verizon Com', 'VZ', 'USD', 1, 1, 1, 1000, 3, 1, 10);
;

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('CSCO.4.45.20', 'CSCO 4.450 `20', 'CSCO',
              CAST(JULIANDAY('2020-01-15') AS INTEGER), 0);

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('DIS.1.125.17', 'DIS 1.125 `17', 'DIS',
              CAST(JULIANDAY('2017-02-15') AS INTEGER), 0);

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('IBM.8.375.19', 'IBM 8.375 `19', 'IBM',
              CAST(JULIANDAY('2019-11-01') AS INTEGER), 0);

INSERT INTO market_t (mnem, display, contr, settl_day, state)
       VALUES ('INTC.2.7.22', 'INTC 2.700 `22', 'INTC',
              CAST(JULIANDAY('2022-12-15') AS INTEGER), 0);

COMMIT
;
