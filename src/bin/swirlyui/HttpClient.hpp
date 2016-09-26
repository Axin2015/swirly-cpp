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
#ifndef SWIRLYUI_HTTPCLIENT_HPP
#define SWIRLYUI_HTTPCLIENT_HPP

#include "Types.hpp"

#include <QNetworkAccessManager>

namespace swirly {
namespace ui {

class Asset;
class Contr;
class Order;
class Exec;
class Posn;

class HttpClient : public QObject {
  Q_OBJECT

 public:
  HttpClient(QObject* parent = nullptr);
  ~HttpClient() noexcept = default;

  void postMarket(const QString& contr, QDate settlDate, MarketState state);

  void postOrder(const QString& contr, QDate settlDate, const QString& ref, const QString& side,
                 Lots lots, Ticks ticks);

 protected:
  void timerEvent(QTimerEvent* event) override;

 signals:
  void refDataComplete();
  void serviceError(const QString& error);
  void updateAsset(const Asset& asset);
  void updateContr(const Contr& contr);
  void updateOrder(const Order& order);
  void updateExec(const Exec& exec);
  void updateTrade(const Exec& trade);
  void updatePosn(const Posn& posn);

 private slots:
  void slotFinished(QNetworkReply* reply);

 private:
  void getRefData();
  void getAccnt();

  void getRefDataReply(QNetworkReply& reply);
  void getAccntReply(QNetworkReply& reply);
  void postMarketReply(QNetworkReply& reply);
  void postOrderReply(QNetworkReply& reply);

  QNetworkAccessManager nam_;
};

} // ui
} // swirly

#endif // SWIRLYUI_HTTPCLIENT_HPP
