/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#include "HttpClient.hxx"

#include "Asset.hxx"
#include "Exec.hxx"
#include "Instr.hxx"
#include "Market.hxx"
#include "Order.hxx"
#include "Posn.hxx"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>

#include <boost/range/adaptor/reversed.hpp>

namespace swirly {
namespace ui {
namespace {

enum : int { GetRefData = 1, GetSess, PostMarket, PostOrder, PutOrder };

} // namespace

HttpClient::HttpClient(QObject* parent)
: Client{parent}
{
    connect(&nam_, &QNetworkAccessManager::finished, this, &HttpClient::slot_finished);
    connect(&nam_, &QNetworkAccessManager::networkAccessibleChanged, this,
            &HttpClient::slot_networkAccessibleChanged);
    get_ref_data();
    startTimer(2000);
}

HttpClient::~HttpClient() = default;

void HttpClient::timerEvent(QTimerEvent* event)
{
    if (errors_ > 0) {
        if (!pending_) {
            errors_ = 0;
            reset();
            get_ref_data();
        }
    } else {
        get_sess();
    }
}

void HttpClient::create_market(const Instr& instr, QDate settl_date)
{
    QNetworkRequest request{QUrl{"http://127.0.0.1:8080/api/market"}};
    request.setAttribute(QNetworkRequest::User, PostMarket);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Swirly-Accnt", "MARAYL");
    request.setRawHeader("Swirly-Perm", "1");

    QJsonObject obj;
    obj["instr"] = instr.symbol();
    obj["settl_date"] = to_json(settl_date);
    obj["state"] = 0;

    QJsonDocument doc(obj);
    nam_.post(request, doc.toJson());
    ++pending_;
}

void HttpClient::create_order(const Instr& instr, QDate settl_date, const QString& ref, Side side,
                              Lots lots, Ticks ticks)
{
    QNetworkRequest request{QUrl{"http://127.0.0.1:8080/api/sess/order"}};
    request.setAttribute(QNetworkRequest::User, PostOrder);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Swirly-Accnt", "MARAYL");
    request.setRawHeader("Swirly-Perm", "2");

    QJsonObject obj;
    obj["instr"] = instr.symbol();
    obj["settl_date"] = to_json(settl_date);
    obj["ref"] = ref;
    obj["side"] = to_json(side);
    obj["lots"] = to_json(lots);
    obj["ticks"] = to_json(ticks);

    QJsonDocument doc(obj);
    nam_.post(request, doc.toJson());
    ++pending_;
}

void HttpClient::cancel_orders(const OrderKeys& keys)
{
    QString str;
    QTextStream out{&str};
    Market market;

    for (const auto& key : keys) {
        const auto id = key.second.count();
        if (key.first != market.id()) {
            if (!str.isEmpty()) {
                put_order(QUrl{str});
                str.clear();
                out.reset();
            }
            market = market_model().find(key.first);
            out << "http://127.0.0.1:8080/api/sess/order/" << market.instr().symbol() //
                << '/' << date_to_iso(market.settl_date())                            //
                << '/' << id;
        } else {
            out << ',' << id;
        }
    }
    if (!str.isEmpty()) {
        put_order(QUrl{str});
    }
}

void HttpClient::slot_finished(QNetworkReply* reply)
{
    --pending_;
    reply->deleteLater();

    const auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (reply->error() != QNetworkReply::NoError) {
        if (status_code.isNull()) {
            ++errors_;
        }
        emit service_error(reply->errorString());
        return;
    }

    const auto attr = reply->request().attribute(QNetworkRequest::User);
    switch (attr.value<int>()) {
    case GetRefData:
        on_ref_data_reply(*reply);
        break;
    case GetSess:
        on_sess_reply(*reply);
        break;
    case PostMarket:
        on_market_reply(*reply);
        break;
    case PostOrder:
        on_order_reply(*reply);
        break;
    case PutOrder:
        on_order_reply(*reply);
        break;
    }
}

void HttpClient::slot_networkAccessibleChanged(
    QNetworkAccessManager::NetworkAccessibility accessible)
{
    switch (accessible) {
    case QNetworkAccessManager::UnknownAccessibility:
        qDebug() << "network accessibility unknown";
        break;
    case QNetworkAccessManager::NotAccessible:
        qDebug() << "network is not accessible";
        break;
    case QNetworkAccessManager::Accessible:
        qDebug() << "network is accessible";
        break;
    };
}

Instr HttpClient::find_instr(const QJsonObject& obj) const
{
    return instr_model().find(from_json<QString>(obj["instr"]));
}

void HttpClient::get_ref_data()
{
    QNetworkRequest request{QUrl{"http://127.0.0.1:8080/api/refdata"}};
    request.setAttribute(QNetworkRequest::User, GetRefData);
    request.setRawHeader("Swirly-Accnt", "MARAYL");
    request.setRawHeader("Swirly-Perm", "2");
    nam_.get(request);
    ++pending_;
}

void HttpClient::get_sess()
{
    QUrl url{"http://127.0.0.1:8080/api/sess"};
    QUrlQuery query;
    query.addQueryItem("limit", QString::number(MaxExecs));
    url.setQuery(query.query());

    QNetworkRequest request{url};
    request.setAttribute(QNetworkRequest::User, GetSess);
    // FIXME: add login dialog or command-line options.
    request.setRawHeader("Swirly-Accnt", "MARAYL");
    request.setRawHeader("Swirly-Perm", "2");
    nam_.get(request);
    ++pending_;
}

void HttpClient::put_order(const QUrl& url)
{
    QNetworkRequest request{url};
    request.setAttribute(QNetworkRequest::User, PutOrder);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Swirly-Accnt", "MARAYL");
    request.setRawHeader("Swirly-Perm", "2");

    nam_.put(request, "{\"lots\":0}");
    ++pending_;
}

void HttpClient::on_ref_data_reply(QNetworkReply& reply)
{
    auto body = reply.readAll();

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(body, &error);
    if (error.error != QJsonParseError::NoError) {
        emit service_error(error.errorString());
        return;
    }

    // New tag for mark and sweep.
    ++tag_;

    const auto obj = doc.object();
    for (const auto elem : obj["assets"].toArray()) {
        const auto asset = Asset::from_json(elem.toObject());
        qDebug().nospace() << "asset: " << asset;
        asset_model().update_row(tag_, asset);
    }
    asset_model().sweep(tag_);
    for (const auto elem : obj["instrs"].toArray()) {
        const auto instr = Instr::from_json(elem.toObject());
        qDebug().nospace() << "instr: " << instr;
        instr_model().update_row(tag_, instr);
    }
    instr_model().sweep(tag_);
    emit ref_data_complete();

    get_sess();
}

void HttpClient::on_sess_reply(QNetworkReply& reply)
{
    auto body = reply.readAll();

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(body, &error);
    if (error.error != QJsonParseError::NoError) {
        emit service_error(error.errorString());
        return;
    }

    // New tag for mark and sweep.
    ++tag_;

    const auto obj = doc.object();
    for (const auto elem : obj["markets"].toArray()) {
        const auto obj = elem.toObject();
        const auto instr = find_instr(obj);
        market_model().update_row(tag_, Market::from_json(instr, obj));
    }
    market_model().sweep(tag_);
    for (const auto elem : obj["orders"].toArray()) {
        const auto obj = elem.toObject();
        const auto instr = find_instr(obj);
        const auto order = Order::from_json(instr, obj);
        if (!order.done()) {
            order_model().update_row(tag_, order);
        } else {
            order_model().remove_row(order);
        }
    }
    order_model().sweep(tag_);
    {
        using boost::adaptors::reverse;
        auto arr = obj["execs"].toArray();
        for (const auto elem : reverse(arr)) {
            const auto obj = elem.toObject();
            const auto instr = find_instr(obj);
            exec_model().update_row(tag_, Exec::from_json(instr, obj));
        }
    }
    for (const auto elem : obj["trades"].toArray()) {
        const auto obj = elem.toObject();
        const auto instr = find_instr(obj);
        trade_model().update_row(tag_, Exec::from_json(instr, obj));
    }
    trade_model().sweep(tag_);
    for (const auto elem : obj["posns"].toArray()) {
        const auto obj = elem.toObject();
        const auto instr = find_instr(obj);
        posn_model().update_row(tag_, Posn::from_json(instr, obj));
    }
    posn_model().sweep(tag_);
}

void HttpClient::on_market_reply(QNetworkReply& reply)
{
    auto body = reply.readAll();

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(body, &error);
    if (error.error != QJsonParseError::NoError) {
        emit service_error(error.errorString());
        return;
    }

    const auto obj = doc.object();
    const auto instr = find_instr(obj);
    market_model().update_row(tag_, Market::from_json(instr, obj));
}

void HttpClient::on_order_reply(QNetworkReply& reply)
{
    auto body = reply.readAll();

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(body, &error);
    if (error.error != QJsonParseError::NoError) {
        emit service_error(error.errorString());
        return;
    }

    const auto obj = doc.object();
    {
        const auto elem = obj["market"];
        if (!elem.isNull()) {
            const auto obj = elem.toObject();
            const auto instr = find_instr(obj);
            market_model().update_row(tag_, Market::from_json(instr, obj));
        }
    }
    for (const auto elem : obj["orders"].toArray()) {
        const auto obj = elem.toObject();
        const auto instr = find_instr(obj);
        const auto order = Order::from_json(instr, obj);
        if (!order.done()) {
            order_model().update_row(tag_, order);
        } else {
            order_model().remove_row(order);
        }
    }
    {
        using boost::adaptors::reverse;
        auto arr = obj["execs"].toArray();
        for (const auto elem : reverse(arr)) {
            const auto obj = elem.toObject();
            const auto instr = find_instr(obj);
            const auto exec = Exec::from_json(instr, obj);
            exec_model().update_row(tag_, exec);
            if (exec.state() == State::Trade) {
                trade_model().update_row(tag_, exec);
            }
        }
    }
    {
        const auto elem = obj["posn"];
        if (!elem.isNull()) {
            const auto obj = elem.toObject();
            const auto instr = find_instr(obj);
            posn_model().update_row(tag_, Posn::from_json(instr, obj));
        }
    }
}

} // namespace ui
} // namespace swirly
