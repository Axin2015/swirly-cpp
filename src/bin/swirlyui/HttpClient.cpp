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

#include "HttpClient.hpp"

#include "Asset.hpp"
#include "Contr.hpp"
#include "Json.hpp"
#include "Market.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

namespace swirly {
namespace ui {
namespace {

enum : int { GetRefData = 1, GetAccnt, PostMarket, PostOrder };

} // anonymous

HttpClient::HttpClient(QObject* parent) : QObject{parent}
{
  connect(&nam_, &QNetworkAccessManager::finished, this, &HttpClient::slotFinished);
  getRefData();
}

void HttpClient::timerEvent(QTimerEvent* event)
{
  qDebug() << "timerEvent";
  getAccnt();
}

void HttpClient::postMarket(const QString& contr, QDate settlDate, MarketState state)
{
  QNetworkRequest request{QUrl{"http://127.0.0.1:8080/market"}};
  request.setAttribute(QNetworkRequest::User, PostMarket);
  request.setRawHeader("Content-Type", "application/json");
  request.setRawHeader("Swirly-Accnt", "MARAYL");
  request.setRawHeader("Swirly-Perm", "1");

  QJsonObject obj;
  obj["contr"] = contr;
  obj["settlDate"] = toJson(settlDate);
  obj["state"] = static_cast<int>(state);

  QJsonDocument doc(obj);
  nam_.post(request, doc.toJson());
}

void HttpClient::postOrder(const QString& contr, QDate settlDate, const QString& ref,
                           const QString& side, Lots lots, Ticks ticks)
{
  QNetworkRequest request{QUrl{"http://127.0.0.1:8080/accnt/order"}};
  request.setAttribute(QNetworkRequest::User, PostOrder);
  request.setRawHeader("Content-Type", "application/json");
  request.setRawHeader("Swirly-Accnt", "MARAYL");
  request.setRawHeader("Swirly-Perm", "2");

  QJsonObject obj;
  obj["contr"] = contr;
  obj["settlDate"] = toJson(settlDate);
  obj["ref"] = ref;
  obj["side"] = side;
  obj["lots"] = lots;
  obj["ticks"] = ticks;

  QJsonDocument doc(obj);
  nam_.post(request, doc.toJson());
}

void HttpClient::slotFinished(QNetworkReply* reply)
{
  qDebug() << "slotFinished";
  reply->deleteLater();

  if (reply->error() != QNetworkReply::NoError) {
    emit notifyError(reply->errorString());
    return;
  }

  const auto attr = reply->request().attribute(QNetworkRequest::User);
  switch (attr.value<int>()) {
  case GetRefData:
    getRefDataReply(reply);
    break;
  case GetAccnt:
    getAccntReply(reply);
    break;
  case PostOrder:
    postOrderReply(reply);
    break;
  }
}

void HttpClient::getRefData()
{
  QNetworkRequest request{QUrl{"http://127.0.0.1:8080/ref"}};
  request.setAttribute(QNetworkRequest::User, GetRefData);
  request.setRawHeader("Swirly-Accnt", "MARAYL");
  request.setRawHeader("Swirly-Perm", "2");
  nam_.get(request);
}

void HttpClient::getAccnt()
{
  QNetworkRequest request{QUrl{"http://127.0.0.1:8080/accnt"}};
  request.setAttribute(QNetworkRequest::User, GetAccnt);
  request.setRawHeader("Swirly-Accnt", "MARAYL");
  request.setRawHeader("Swirly-Perm", "2");
  nam_.get(request);
}

void HttpClient::getRefDataReply(QNetworkReply* reply)
{
  auto body = reply->readAll();
  qDebug() << "getRefDataReply:" << body;

  QJsonParseError error;
  const auto doc = QJsonDocument::fromJson(body, &error);
  if (error.error != QJsonParseError::NoError) {
    emit notifyError(error.errorString());
    return;
  }

  const auto obj = doc.object();
  for (const auto elem : obj["assets"].toArray()) {
    const auto asset = Asset::parse(elem.toObject());
    qDebug() << asset;
    emit notifyAsset(asset);
  }
  for (const auto elem : obj["contrs"].toArray()) {
    const auto contr = Contr::parse(elem.toObject());
    qDebug() << contr;
    emit notifyContr(contr);
  }

  //postMarket("EURUSD", 20160923, 0);
  //postOrder("EURUSD", 20160923, "test", "Buy", 10, 12345);
  getAccnt();
  startTimer(2000);
}

void HttpClient::getAccntReply(QNetworkReply* reply)
{
  auto body = reply->readAll();
  qDebug() << "getAccntReply:" << body;

  QJsonParseError error;
  const auto doc = QJsonDocument::fromJson(body, &error);
  if (error.error != QJsonParseError::NoError) {
    emit notifyError(error.errorString());
    return;
  }

  const auto obj = doc.object();
  for (const auto elem : obj["markets"].toArray()) {
    const auto market = Market::parse(elem.toObject());
    qDebug() << market;
  }
}

void HttpClient::postMarketReply(QNetworkReply* reply)
{
  auto body = reply->readAll();
  qDebug() << "postMarketReply:" << body;
}

void HttpClient::postOrderReply(QNetworkReply* reply)
{
  auto body = reply->readAll();
  qDebug() << "postOrderReply:" << body;
}

} // ui
} // swirly
