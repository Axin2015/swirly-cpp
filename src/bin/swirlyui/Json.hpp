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
#ifndef SWIRLYUI_JSON_HPP
#define SWIRLYUI_JSON_HPP

#include <QDate>
#include <QJsonValue>

namespace swirly {
namespace ui {

template <typename ValueT>
struct JsonTraits;

template <>
struct JsonTraits<int> {
  static int fromJson(const QJsonValue& value) { return value.toInt(); }
  static QJsonValue toJson(int value) { return value; }
};

template <>
struct JsonTraits<unsigned> {
  static unsigned fromJson(const QJsonValue& value) { return static_cast<unsigned>(value.toInt()); }
  static QJsonValue toJson(unsigned value) { return static_cast<int>(value); }
};

template <>
struct JsonTraits<qint64> {
  static qint64 fromJson(const QJsonValue& value) { return value.toDouble(); }
  static QJsonValue toJson(qint64 value) { return value; }
};

template <>
struct JsonTraits<QString> {
  static QString fromJson(const QJsonValue& value) { return value.toString(); }
  static QJsonValue toJson(const QString& value) { return value; }
};

template <>
struct JsonTraits<QDate> {
  static QDate fromJson(const QJsonValue& value)
  {
    const auto n = value.toInt();
    const auto y = n / 10000;
    const auto m = (n / 100 % 100) - 1;
    const auto d = n % 100;
    return {y, m, d};
  }
  static QJsonValue toJson(const QDate& value)
  {
    return value.year() * 10000 + value.month() * 100 + value.day();
  }
};

template <>
struct JsonTraits<QDateTime> {
  static QDateTime fromJson(const QJsonValue& value)
  {
    return QDateTime::fromMSecsSinceEpoch(value.toDouble());
  }
  static QJsonValue toJson(const QDateTime& value) { return value.toMSecsSinceEpoch(); }
};

template <typename ValueT>
ValueT fromJson(const QJsonValue& value)
{
  return JsonTraits<ValueT>::fromJson(value);
}

template <typename ValueT>
QJsonValue toJson(const ValueT& value)
{
  return JsonTraits<ValueT>::toJson(value);
}

} // ui
} // swirly

#endif // SWIRLYUI_JSON_HPP
