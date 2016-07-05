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

CREATE TABLE state_t (
  id INT NOT NULL PRIMARY KEY,
  mnem CHAR(16) NOT NULL UNIQUE
)
;

INSERT INTO state_t (id, mnem) VALUES (1, 'NEW')
;
INSERT INTO state_t (id, mnem) VALUES (2, 'REVISE')
;
INSERT INTO state_t (id, mnem) VALUES (3, 'CANCEL')
;
INSERT INTO state_t (id, mnem) VALUES (4, 'TRADE')
;

CREATE TABLE side_t (
  id INT NOT NULL PRIMARY KEY,
  mnem CHAR(16) NOT NULL UNIQUE
)
;

INSERT INTO side_t (id, mnem) VALUES (1, 'BUY')
;
INSERT INTO side_t (id, mnem) VALUES (-1, 'SELL')
;

CREATE TABLE direct_t (
  id INT NOT NULL PRIMARY KEY,
  mnem CHAR(16) NOT NULL UNIQUE
)
;

INSERT INTO direct_t (id, mnem) VALUES (1, 'PAID')
;
INSERT INTO direct_t (id, mnem) VALUES (-1, 'GIVEN')
;

CREATE TABLE liqind_t (
  id INT NOT NULL PRIMARY KEY,
  mnem CHAR(16) NOT NULL UNIQUE
)
;

INSERT INTO liqind_t (id, mnem) VALUES (1, 'MAKER')
;
INSERT INTO liqind_t (id, mnem) VALUES (2, 'TAKER')
;

CREATE TABLE asset_type_t (
  id INT NOT NULL PRIMARY KEY,
  mnem CHAR(16) NOT NULL UNIQUE
)
;

INSERT INTO asset_type_t (id, mnem) VALUES (1, 'COMMODITY')
;
INSERT INTO asset_type_t (id, mnem) VALUES (2, 'CORPORATE')
;
INSERT INTO asset_type_t (id, mnem) VALUES (3, 'CURRENCY')
;
INSERT INTO asset_type_t (id, mnem) VALUES (4, 'EQUITY')
;
INSERT INTO asset_type_t (id, mnem) VALUES (5, 'GOVERNMENT')
;
INSERT INTO asset_type_t (id, mnem) VALUES (6, 'INDEX')
;

CREATE TABLE asset_t (
  mnem CHAR(16) NOT NULL PRIMARY KEY,
  display VARCHAR(64) NOT NULL UNIQUE,
  type_id INT NOT NULL,

  FOREIGN KEY (type_id) REFERENCES asset_type_t (id)
)
;

CREATE TABLE contr_t (
  mnem CHAR(16) NOT NULL PRIMARY KEY,
  display VARCHAR(64) NOT NULL,
  asset CHAR(16) NOT NULL,
  ccy CHAR(16) NOT NULL,
  lot_numer INT NOT NULL,
  lot_denom INT NOT NULL,
  tick_numer INT NOT NULL,
  tick_denom INT NOT NULL,
  pip_dp INT NOT NULL,
  min_lots BIGINT NOT NULL DEFAULT 1,
  max_lots BIGINT NOT NULL,

  FOREIGN KEY (asset) REFERENCES asset_t (mnem),
  FOREIGN KEY (ccy) REFERENCES asset_t (mnem)
)
;

CREATE TABLE market_t (
  mnem CHAR(16) NOT NULL PRIMARY KEY,
  display VARCHAR(64) NOT NULL,
  contr CHAR(16) NOT NULL,
  settl_day INT NULL DEFAULT NULL,
  expiry_day INT NULL DEFAULT NULL,
  state INT NOT NULL DEFAULT 0,
  last_lots BIGINT NULL DEFAULT NULL,
  last_ticks BIGINT NULL DEFAULT NULL,
  last_time BIGINT NULL DEFAULT NULL,

  FOREIGN KEY (contr) REFERENCES contr_t (mnem)
)
;

CREATE TABLE order_t (
  accnt CHAR(16) NOT NULL,
  market CHAR(16) NOT NULL,
  contr CHAR(16) NOT NULL,
  settl_day INT NULL DEFAULT NULL,
  id BIGINT NOT NULL,
  ref VARCHAR(64) NULL DEFAULT NULL,
  state_id INT NOT NULL,
  side_id INT NOT NULL,
  lots BIGINT NOT NULL,
  ticks BIGINT NOT NULL,
  resd BIGINT NOT NULL,
  exec BIGINT NOT NULL,
  cost BIGINT NOT NULL,
  last_lots BIGINT NULL DEFAULT NULL,
  last_ticks BIGINT NULL DEFAULT NULL,
  min_lots BIGINT NOT NULL DEFAULT 1,
  archive TINYINT(1) NOT NULL DEFAULT 0,
  pecan TINYINT(1) NOT NULL DEFAULT 0,
  created BIGINT NOT NULL,
  modified BIGINT NOT NULL,

  PRIMARY KEY (market, id),
  CONSTRAINT order_accnt_ref_unq UNIQUE (accnt, ref),

  FOREIGN KEY (market) REFERENCES market_t (mnem),
  FOREIGN KEY (contr) REFERENCES contr_t (mnem),
  FOREIGN KEY (state_id) REFERENCES state_t (id),
  FOREIGN KEY (side_id) REFERENCES side_t (id)
)
;

CREATE INDEX order_accnt_idx ON order_t (accnt);
CREATE INDEX order_resd_idx ON order_t (resd);
CREATE INDEX order_archive_idx ON order_t (archive);

CREATE TABLE exec_t (
  accnt CHAR(16) NOT NULL,
  market CHAR(16) NOT NULL,
  contr CHAR(16) NOT NULL,
  settl_day INT NULL DEFAULT NULL,
  id BIGINT NOT NULL,
  ref VARCHAR(64) NULL DEFAULT NULL,
  order_id BIGINT NULL DEFAULT NULL,
  state_id INT NOT NULL,
  side_id INT NOT NULL,
  lots BIGINT NOT NULL,
  ticks BIGINT NOT NULL,
  resd BIGINT NOT NULL,
  exec BIGINT NOT NULL,
  cost BIGINT NOT NULL,
  last_lots BIGINT NULL DEFAULT NULL,
  last_ticks BIGINT NULL DEFAULT NULL,
  min_lots BIGINT NOT NULL DEFAULT 1,
  match_id BIGINT NULL DEFAULT NULL,
  liqind_id INT NULL DEFAULT NULL,
  cpty CHAR(16) NULL DEFAULT NULL,
  archive TINYINT(1) NOT NULL DEFAULT 0,
  created BIGINT NOT NULL,
  modified BIGINT NOT NULL,

  PRIMARY KEY (market, id),

  FOREIGN KEY (market, order_id) REFERENCES order_t (market, id),
  FOREIGN KEY (contr) REFERENCES contr_t (mnem),
  FOREIGN KEY (state_id) REFERENCES state_t (id),
  FOREIGN KEY (side_id) REFERENCES side_t (id),
  FOREIGN KEY (liqind_id) REFERENCES liqind_t (id)
)
;

CREATE INDEX exec_accnt_idx ON exec_t (accnt);
CREATE INDEX exec_state_idx ON exec_t (state_id);
CREATE INDEX exec_archive_idx ON exec_t (archive);

CREATE TRIGGER before_insert_on_exec1
  BEFORE INSERT ON exec_t
  WHEN NEW.order_id IS NOT NULL
  AND NEW.state_id = 1
  BEGIN
    INSERT INTO order_t (
      accnt,
      market,
      contr,
      settl_day,
      id,
      ref,
      state_id,
      side_id,
      lots,
      ticks,
      resd,
      exec,
      cost,
      last_lots,
      last_ticks,
      min_lots,
      archive,
      created,
      modified
    ) VALUES (
      NEW.accnt,
      NEW.market,
      NEW.contr,
      NEW.settl_day,
      NEW.order_id,
      NEW.ref,
      NEW.state_id,
      NEW.side_id,
      NEW.lots,
      NEW.ticks,
      NEW.resd,
      NEW.exec,
      NEW.cost,
      NEW.last_lots,
      NEW.last_ticks,
      NEW.min_lots,
      0,
      NEW.created,
      NEW.modified
    );
  END
;

CREATE TRIGGER before_insert_on_exec2
  BEFORE INSERT ON exec_t
  WHEN NEW.order_id IS NOT NULL
  AND NEW.state_id != 1
  BEGIN
    UPDATE order_t
    SET
      state_id = NEW.state_id,
      lots = NEW.lots,
      resd = NEW.resd,
      exec = NEW.exec,
      cost = NEW.cost,
      last_lots = NEW.last_lots,
      last_ticks = NEW.last_ticks,
      modified = NEW.modified
    WHERE id = NEW.order_id;
  END
;

CREATE TRIGGER before_insert_on_exec3
  BEFORE INSERT ON exec_t
  WHEN NEW.state_id = 4
  BEGIN
    UPDATE market_t
    SET
      last_lots = NEW.last_lots,
      last_ticks = NEW.last_ticks,
      last_time = NEW.created
    WHERE mnem = NEW.market;
  END
;

CREATE VIEW asset_v AS
  SELECT
    a.mnem,
    a.display,
    t.mnem type
  FROM asset_t a
  LEFT OUTER JOIN asset_type_t t
  ON a.type_id = t.id
;

CREATE VIEW contr_v AS
  SELECT
    c.mnem,
    c.display,
    a.type,
    c.asset,
    c.ccy,
    c.lot_numer,
    c.lot_denom,
    c.tick_numer,
    c.tick_denom,
    c.pip_dp,
    c.min_lots,
    c.max_lots
  FROM contr_t c
  LEFT OUTER JOIN asset_v a
  ON c.asset = a.mnem
;

CREATE VIEW market_v AS
  SELECT
    m.mnem,
    m.display,
    m.contr,
    m.settl_day,
    m.expiry_day,
    m.state,
    m.last_lots,
    m.last_ticks,
    m.last_time,
    MAX(e.order_id) max_order_id,
    MAX(e.id) max_exec_id
  FROM market_t m
  LEFT OUTER JOIN exec_t e
  ON m.mnem = e.market
  GROUP BY m.mnem
;

CREATE VIEW order_v AS
  SELECT
    o.accnt,
    o.market,
    o.contr,
    o.settl_day,
    o.id,
    o.ref,
    s.mnem state,
    a.mnem side,
    o.lots,
    o.ticks,
    o.resd,
    o.exec,
    o.cost,
    o.last_lots,
    o.last_ticks,
    o.min_lots,
    o.pecan,
    o.created,
    o.modified
  FROM order_t o
  LEFT OUTER JOIN state_t s
  ON o.state_id = s.id
  LEFT OUTER JOIN side_t a
  ON o.side_id = a.id
;

CREATE VIEW exec_v AS
  SELECT
    e.accnt,
    e.contr,
    e.settl_day,
    e.id,
    e.ref,
    e.order_id,
    s.mnem state,
    a.mnem side,
    e.lots,
    e.ticks,
    e.resd,
    e.exec,
    e.cost,
    e.last_lots,
    e.last_ticks,
    e.min_lots,
    e.match_id,
    r.mnem liqind,
    e.cpty,
    e.archive,
    e.created,
    e.modified
  FROM exec_t e
  LEFT OUTER JOIN state_t s
  ON e.state_id = s.id
  LEFT OUTER JOIN side_t a
  ON e.side_id = a.id
  LEFT OUTER JOIN liqind_t r
  ON e.liqind_id = r.id
;

CREATE VIEW posn_v AS
  SELECT
    e.accnt,
    e.contr,
    e.settl_day,
    e.side_id,
    SUM(e.last_lots) lots,
    SUM(e.last_lots * e.last_ticks) cost
  FROM exec_t e
  WHERE e.state_id = 4
  GROUP BY e.accnt, e.contr, e.settl_day, e.side_id
;

COMMIT
;
