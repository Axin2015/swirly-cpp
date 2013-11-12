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

CREATE TABLE status (
  id INTEGER PRIMARY KEY,
  mnem TEXT NOT NULL UNIQUE
)
;

INSERT INTO status (id, mnem) VALUES (1, 'NEW')
;
INSERT INTO status (id, mnem) VALUES (2, 'REVISED')
;
INSERT INTO status (id, mnem) VALUES (3, 'CANCELLED')
;
INSERT INTO status (id, mnem) VALUES (4, 'REJECTED')
;
INSERT INTO status (id, mnem) VALUES (5, 'TRADED')
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
  rev INTEGER NOT NULL,
  status INTEGER NOT NULL REFERENCES status (id),
  trader INTEGER NOT NULL REFERENCES trader (id),
  accnt INTEGER NOT NULL REFERENCES accnt (id),
  contr INTEGER NOT NULL REFERENCES contr (id),
  settl_date INTEGER NOT NULL,
  ref TEXT NULL,
  action INTEGER NOT NULL REFERENCES action (id),
  ticks INTEGER NOT NULL,
  lots INTEGER NOT NULL,
  resd INTEGER NOT NULL,
  exec INTEGER NOT NULL,
  last_ticks INTEGER NOT NULL,
  last_lots INTEGER NOT NULL,
  min INTEGER NOT NULL,
  flags INTEGER NOT NULL,
  archive INTEGER NOT NULL,
  created INTEGER NOT NULL,
  modified INTEGER NOT NULL,
  CONSTRAINT order_trader_ref_uq UNIQUE (trader, ref)
)
;

CREATE VIEW order_v AS
  SELECT
  o.id,
  o.rev,
  s.mnem status,
  t.mnem trader,
  a.mnem accnt,
  c.mnem contr,
  o.settl_date,
  o.ref,
  o.action,
  o.ticks,
  o.lots,
  o.resd,
  o.exec,
  o.last_ticks,
  o.last_lots,
  o.min,
  o.flags,
  o.archive,
  o.created,
  o.modified
  FROM order_ o
  INNER JOIN status s
  ON o.status = s.id
  INNER JOIN trader t
  ON o.trader = t.id
  INNER JOIN accnt a
  ON o.accnt = a.id
  INNER JOIN contr c
  ON o.contr = c.id
;

CREATE TABLE history (
  id INTEGER NOT NULL REFERENCES order_ (id),
  rev INTEGER NOT NULL,
  status INTEGER NOT NULL,
  lots INTEGER NOT NULL,
  resd INTEGER NOT NULL,
  exec INTEGER NOT NULL,
  last_ticks INTEGER NOT NULL,
  last_lots INTEGER NOT NULL,
  modified INTEGER NOT NULL,
  PRIMARY KEY (id, rev)
)
;

CREATE VIEW history_v AS
  SELECT
  o.id,
  o.rev,
  s.mnem status,
  o.lots,
  o.resd,
  o.exec,
  o.last_ticks,
  o.last_lots,
  o.modified
  FROM history o
  INNER JOIN status s
  ON o.status = s.id
;

CREATE TRIGGER after_insert_on_order
  AFTER INSERT ON order_
BEGIN
  INSERT INTO history (id, rev, status, lots, resd, exec, last_ticks, last_lots, modified)
  VALUES (new.id, new.rev, new.status, new.lots, new.resd, new.exec, new.last_ticks,
          new.last_lots, new.modified);
END
;

CREATE TRIGGER after_update_on_order
  AFTER UPDATE ON order_
  WHEN new.archive = 0
BEGIN
  INSERT INTO history (id, rev, status, lots, resd, exec, last_ticks, last_lots, modified)
  VALUES (new.id, new.rev, new.status, new.lots, new.resd, new.exec, new.last_ticks,
          new.last_lots, new.modified);
END
;

CREATE TABLE trade (
  id INTEGER PRIMARY KEY,
  order_ INTEGER NOT NULL REFERENCES order_ (id),
  rev INTEGER NOT NULL,
  match INTEGER NOT NULL,
  role INTEGER NOT NULL REFERENCES role (id),
  cpty INTEGER NOT NULL REFERENCES accnt (id),
  archive INTEGER NOT NULL,
  created INTEGER NOT NULL,
  modified INTEGER NOT NULL,
  CONSTRAINT trade_match_role_uq
  UNIQUE (match, role)
)
;

CREATE VIEW trade_v AS
  SELECT
  t.id,
  t.order_,
  t.rev,
  h.status,
  o.trader,
  o.accnt,
  o.contr,
  o.settl_date,
  o.ref,
  o.action,
  o.ticks,
  h.lots,
  h.resd,
  h.exec,
  h.last_ticks,
  h.last_lots,
  t.match,
  t.role,
  t.cpty,
  t.archive,
  t.created,
  t.modified
  FROM trade t
  INNER JOIN order_ o
  ON t.order_ = h.id
  INNER JOIN history h
  ON t.order_ = h.id
  AND t.rev = h.rev
;

CREATE VIEW posn_v AS
  SELECT
  o.accnt,
  o.contr,
  o.settl_date,
  o.action,
  SUM(h.last_lots * h.last_ticks) licks,
  SUM(h.last_lots) lots
  FROM trade t
  INNER JOIN order_ o
  ON t.order_ = h.id
  INNER JOIN history h
  ON t.order_ = h.id
  AND t.rev = h.rev
  GROUP BY o.accnt, o.contr, o.settl_date, o.action
;

COMMIT TRANSACTION
;
