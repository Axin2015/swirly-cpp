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

  void createMarket(const Contr& contr, QDate settlDate) override;

  void createOrder(const Contr& contr, QDate settlDate, const QString& ref, Side side, Lots lots,
                   Ticks ticks) override;

  void cancelOrders(const OrderKeys& keys) override;

 signals:

 protected:
  void timerEvent(QTimerEvent* event) override;

 private slots:
  void slotFinished(QNetworkReply* reply);

  void slotNetworkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);

 private:
  Contr findContr(const QJsonObject& obj) const;

  void getRefData();
  void getAccnt();
  void putOrder(const QUrl& url);

  void onRefDataReply(QNetworkReply& reply);
  void onAccntReply(QNetworkReply& reply);
  void onMarketReply(QNetworkReply& reply);
  void onOrderReply(QNetworkReply& reply);

  QNetworkAccessManager nam_;
  std::uint64_t tag_{0};
  int errors_{0};
  int pending_{0};
};

} // ui
} // swirly

#endif // SWIRLYUI_HTTPCLIENT_HPP
