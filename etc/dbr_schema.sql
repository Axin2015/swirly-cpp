--  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
--
--  This file is part of Doobry written by Mark Aylett.
--
--  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
--  General Public License as published by the Free Software Foundation; either version 2 of the
--  License, or (at your option) any later version.
--
--  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
--  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
--  General Public License for more details.
--
--  You should have received a copy of the GNU General Public License along with this program; if
--  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
--  02110-1301 USA.

-- Use ';' on single line to terminate statement.
PRAGMA foreign_keys = ON
;

BEGIN TRANSACTION
;

CREATE TABLE state (
  id INTEGER PRIMARY KEY,
  mnem TEXT NOT NULL UNIQUE
)
;

INSERT INTO state (id, mnem) VALUES (1, 'NEW')
;
INSERT INTO state (id, mnem) VALUES (2, 'REVISE')
;
INSERT INTO state (id, mnem) VALUES (3, 'CANCEL')
;
INSERT INTO state (id, mnem) VALUES (4, 'TRADE')
;

CREATE TABLE action (
  id INTEGER PRIMARY KEY,
  mnem TEXT NOT NULL UNIQUE
)
;

INSERT INTO action (id, mnem) VALUES (1, 'BUY')
;
INSERT INTO action (id, mnem) VALUES (-1, 'SELL')
;

CREATE TABLE direct (
  id INTEGER PRIMARY KEY,
  mnem TEXT NOT NULL UNIQUE
)
;

INSERT INTO direct (id, mnem) VALUES (1, 'PAID')
;
INSERT INTO direct (id, mnem) VALUES (-1, 'GIVEN')
;

CREATE TABLE role (
  id INTEGER PRIMARY KEY,
  mnem TEXT NOT NULL UNIQUE
)
;

INSERT INTO role (id, mnem) VALUES (1, 'MAKER')
;
INSERT INTO role (id, mnem) VALUES (2, 'TAKER')
;

CREATE TABLE asset_type (
  mnem TEXT PRIMARY KEY
)
;

CREATE TABLE asset (
  mnem TEXT PRIMARY KEY,
  display TEXT NOT NULL UNIQUE,
  type TEXT NOT NULL REFERENCES asset_type (mnem)
)
;

CREATE TABLE contr (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  mnem TEXT NOT NULL UNIQUE,
  display TEXT NOT NULL UNIQUE,
  asset TEXT NOT NULL REFERENCES asset (mnem),
  ccy TEXT NOT NULL REFERENCES asset (mnem),
  tick_numer INTEGER NOT NULL,
  tick_denom INTEGER NOT NULL,
  pip_dp INTEGER NOT NULL,
  lot_numer INTEGER NOT NULL,
  lot_denom INTEGER NOT NULL,
  min_lots INTEGER NOT NULL,
  max_lots INTEGER NOT NULL
)
;

CREATE VIEW contr_v AS
  SELECT
  c.id,
  c.mnem,
  c.display,
  a.type asset_type,
  c.asset,
  c.ccy,
  c.tick_numer,
  c.tick_denom,
  c.pip_dp,
  c.lot_numer,
  c.lot_denom,
  c.min_lots,
  c.max_lots
  FROM contr c
  INNER JOIN asset a
  ON c.asset = a.mnem
;

CREATE TABLE trader (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  mnem TEXT NOT NULL UNIQUE,
  display TEXT NOT NULL UNIQUE,
  email TEXT NOT NULL UNIQUE,
  title TEXT NULL,
  given TEXT NULL,
  family TEXT NULL
)
;

CREATE VIEW trader_v AS
  SELECT
  t.id,
  t.mnem,
  t.display,
  t.email
  FROM trader t
;

CREATE TABLE accnt (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  mnem TEXT NOT NULL UNIQUE,
  display TEXT NOT NULL UNIQUE,
  email TEXT NOT NULL UNIQUE
)
;

CREATE VIEW accnt_v AS
  SELECT
  a.id,
  a.mnem,
  a.display,
  a.email
  FROM accnt a
;

CREATE TABLE memb (
  trader INTEGER NOT NULL REFERENCES trader (id),
  accnt INTEGER NOT NULL REFERENCES accnt (id),
  PRIMARY KEY (trader, accnt)
)
;

CREATE VIEW memb_v AS
  SELECT
  a.mnem accnt,
  t.mnem trader
  FROM memb m
  INNER JOIN accnt a
  ON m.accnt = a.id
  INNER JOIN trader t
  ON m.trader = t.id
  ORDER BY m.accnt
;

CREATE TABLE order_ (
  id INTEGER PRIMARY KEY,
  trader INTEGER NOT NULL REFERENCES trader (id),
  accnt INTEGER NOT NULL REFERENCES accnt (id),
  contr INTEGER NOT NULL REFERENCES contr (id),
  settl_date INTEGER NOT NULL,
  ref TEXT NULL,
  state INTEGER NOT NULL REFERENCES state (id),
  action INTEGER NOT NULL REFERENCES action (id),
  ticks INTEGER NOT NULL,
  lots INTEGER NOT NULL,
  resd INTEGER NOT NULL,
  exec INTEGER NOT NULL,
  last_ticks INTEGER NOT NULL,
  last_lots INTEGER NOT NULL,
  min_lots INTEGER NOT NULL,
  created INTEGER NOT NULL,
  modified INTEGER NOT NULL,
  CONSTRAINT order_trader_ref_uq UNIQUE (trader, ref)
)
;

CREATE VIEW order_v AS
  SELECT
  o.id,
  t.mnem trader,
  a.mnem accnt,
  c.mnem contr,
  o.settl_date,
  o.ref,
  s.mnem state,
  n.mnem action,
  o.ticks,
  o.lots,
  o.resd,
  o.exec,
  o.last_ticks,
  o.last_lots,
  o.min_lots,
  o.created,
  o.modified
  FROM order_ o
  INNER JOIN trader t
  ON o.trader = t.id
  INNER JOIN accnt a
  ON o.accnt = a.id
  INNER JOIN contr c
  ON o.contr = c.id
  INNER JOIN state s
  ON o.state = s.id
  INNER JOIN action n
  ON o.action = n.id
;

CREATE TABLE exec (
  id INTEGER PRIMARY KEY,
  order_ INTEGER NOT NULL REFERENCES order_ (id),
  trader INTEGER NOT NULL REFERENCES trader (id),
  accnt INTEGER NOT NULL REFERENCES accnt (id),
  contr INTEGER NOT NULL REFERENCES contr (id),
  settl_date INTEGER NOT NULL,
  ref TEXT NULL,
  state INTEGER NOT NULL REFERENCES state (id),
  action INTEGER NOT NULL REFERENCES action (id),
  ticks INTEGER NOT NULL,
  lots INTEGER NOT NULL,
  resd INTEGER NOT NULL,
  exec INTEGER NOT NULL,
  last_ticks INTEGER NOT NULL,
  last_lots INTEGER NOT NULL,
  min_lots INTEGER NOT NULL,
  match INTEGER NULL,
  acked INTEGER NOT NULL,
  role INTEGER NULL REFERENCES role (id),
  cpty INTEGER NULL REFERENCES accnt (id),
  created INTEGER NOT NULL,
  modified INTEGER NOT NULL
)
;

CREATE TRIGGER before_insert_on_exec
  BEFORE INSERT ON exec
  WHEN new.state = 1
BEGIN
  INSERT INTO order_ (
    id,
    trader,
    accnt,
    contr,
    settl_date,
    ref,
    state,
    action,
    ticks,
    lots,
    resd,
    exec,
    last_ticks,
    last_lots,
    min_lots,
    created,
    modified
  ) VALUES (
    new.order_,
    new.trader,
    new.accnt,
    new.contr,
    new.settl_date,
    new.ref,
    new.state,
    new.action,
    new.ticks,
    new.lots,
    new.resd,
    new.exec,
    new.last_ticks,
    new.last_lots,
    new.min_lots,
    new.created,
    new.modified
  );
END
;

CREATE TRIGGER before_update_on_exec
  BEFORE INSERT ON exec
  WHEN new.state != 1
BEGIN
  UPDATE order_
  SET
    state = new.state,
    lots = new.lots,
    resd = new.resd,
    exec = new.exec,
    last_ticks = new.last_ticks,
    last_lots = new.last_lots,
    modified = new.modified
  WHERE id = new.order_;
END
;

CREATE VIEW exec_v AS
  SELECT
  e.id,
  e.order_,
  t.mnem trader,
  a.mnem accnt,
  c.mnem contr,
  e.settl_date,
  e.ref,
  s.mnem state,
  n.mnem action,
  e.ticks,
  e.lots,
  e.resd,
  e.exec,
  e.last_ticks,
  e.last_lots,
  e.min_lots,
  e.match,
  e.acked,
  r.mnem role,
  p.mnem cpty,
  e.created,
  e.modified
  FROM exec e
  INNER JOIN trader t
  ON e.trader = t.id
  INNER JOIN accnt a
  ON e.accnt = a.id
  INNER JOIN contr c
  ON e.contr = c.id
  INNER JOIN state s
  ON e.state = s.id
  INNER JOIN action n
  ON e.action = n.id
  INNER JOIN role r
  ON e.role = r.id
  INNER JOIN accnt p
  ON e.cpty = p.id
;

CREATE VIEW trade_v AS
  SELECT
  e.id,
  e.order_,
  e.trader,
  e.accnt,
  e.contr,
  e.settl_date,
  e.ref,
  e.action,
  e.ticks,
  e.lots,
  e.resd,
  e.exec,
  e.last_ticks,
  e.last_lots,
  e.min_lots,
  e.match,
  e.acked,
  e.role,
  e.cpty,
  e.created,
  e.modified
  FROM exec e
  WHERE e.state = 4
;

CREATE VIEW posn_v AS
  SELECT
  e.accnt,
  e.contr,
  e.settl_date,
  e.action,
  SUM(e.last_lots * e.last_ticks) licks,
  SUM(e.last_lots) lots
  FROM exec e
  WHERE e.state = 4
  GROUP BY e.accnt, e.contr, e.settl_date, e.action
;

COMMIT TRANSACTION
;
