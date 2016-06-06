/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <swirly/elm/Journ.hpp>

#include <swirly/ash/Log.hpp>

using namespace std;

namespace swirly {
namespace {
class MultiPart {
 public:
  explicit MultiPart(Journ& journ) noexcept : journ_{journ} {}
  ~MultiPart() noexcept
  {
    if (!done_) {
      journ_.reset();
    }
  }
  void done() noexcept { done_ = true; }

 private:
  Journ& journ_;
  bool done_{false};
};
} // anonymous

Journ::~Journ() noexcept = default;

void Journ::createExec(ArrayView<ConstExecPtr> execs)
{
  if (execs.size() > 1) {
    doCreateExec(*execs.front(), More::Yes);
    MultiPart mp{*this};
    for (const auto& exec : makeArrayView(&execs[1], execs.size() - 2)) {
      doCreateExec(*exec, More::Yes);
    }
    doCreateExec(*execs.back(), More::No);
    mp.done();
  } else {
    doCreateExec(*execs.front(), More::No);
  }
}

void Journ::archiveOrder(Mnem market, ArrayView<Iden> ids, Millis modified)
{
  if (ids.size() > 1) {
    doArchiveOrder(market, ids.front(), modified, More::Yes);
    MultiPart mp{*this};
    for (const auto id : makeArrayView(&ids[1], ids.size() - 2)) {
      doArchiveOrder(market, id, modified, More::Yes);
    }
    doArchiveOrder(market, ids.back(), modified, More::No);
    mp.done();
  } else {
    doArchiveOrder(market, ids.front(), modified, More::No);
  }
}

void Journ::archiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified)
{
  if (ids.size() > 1) {
    doArchiveTrade(market, ids.front(), modified, More::Yes);
    MultiPart mp{*this};
    for (const auto id : makeArrayView(&ids[1], ids.size() - 2)) {
      doArchiveTrade(market, id, modified, More::Yes);
    }
    doArchiveTrade(market, ids.back(), modified, More::No);
    mp.done();
  } else {
    doArchiveTrade(market, ids.front(), modified, More::No);
  }
}

} // swirly
