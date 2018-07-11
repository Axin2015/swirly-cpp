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
#ifndef SWIRLYUI_HTTPCLIENT_HXX
#define SWIRLYUI_HTTPCLIENT_HXX

#include "Client.hxx"

#include <QNetworkAccessManager>

namespace swirly {
namespace ui {

class HttpClient : public Client {
    Q_OBJECT

  public:
    HttpClient(QObject* parent = nullptr);
    ~HttpClient() override;

    void create_market(const Instr& instr, QDate settl_date) override;

    void create_order(const Instr& instr, QDate settl_date, const QString& ref, Side side,
                      Lots lots, Ticks ticks) override;

    void cancel_orders(const OrderKeys& keys) override;

  signals:

  protected:
    void timerEvent(QTimerEvent* event) override;

  private slots:
    void slot_finished(QNetworkReply* reply);

    void slot_networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);

  private:
    Instr find_instr(const QJsonObject& obj) const;

    void get_ref_data();
    void get_accnt();
    void put_order(const QUrl& url);

    void on_ref_data_reply(QNetworkReply& reply);
    void on_sess_reply(QNetworkReply& reply);
    void on_market_reply(QNetworkReply& reply);
    void on_order_reply(QNetworkReply& reply);

    QNetworkAccessManager nam_;
    std::uint64_t tag_{0};
    int errors_{0};
    int pending_{0};
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_HTTPCLIENT_HXX
