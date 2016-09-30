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

#include "Client.hpp"

#include <QNetworkAccessManager>

namespace swirly {
namespace ui {

class HttpClient : public Client {
  Q_OBJECT

 public:
  HttpClient(QObject* parent = nullptr);
  ~HttpClient() noexcept override;

  void postMarket(const QString& contr, QDate settlDate, MarketState state);

  void postOrder(const QString& contr, QDate settlDate, const QString& ref, const QString& side,
                 Lots lots, Ticks ticks);

 protected:
  void timerEvent(QTimerEvent* event) override;

 signals:

 private slots:
  void slotFinished(QNetworkReply* reply);

 private:
  Contr findContr(const QJsonObject& obj) const;

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
