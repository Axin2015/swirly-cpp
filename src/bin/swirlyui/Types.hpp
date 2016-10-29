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
#ifndef SWIRLYUI_TYPES_HPP
#define SWIRLYUI_TYPES_HPP

#include <swirly/fin/BasicTypes.hpp>
#include <swirly/fin/Conv.hpp>
#include <swirly/fin/Limits.hpp>

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Enum.hpp>
#include <swirly/util/Limits.hpp>

#include <QDate>
#include <QDebug>
#include <QJsonValue>

// This is acceptable in private header that is only visible to and under the control of the Swirly
// build.

namespace swirly {
namespace ui {

constexpr std::size_t MaxExecs{8};

template <typename ValueT, typename EnableT = void>
struct TypeTraits;

template <>
struct TypeTraits<int> {
  static int fromJson(const QJsonValue& value) { return value.toInt(); }
  static int fromVariant(const QVariant& value) { return value.toInt(); }
  static QJsonValue toJson(int value) { return value; }
  static QVariant toVariant(int value) { return value; }
};

template <>
struct TypeTraits<unsigned> {
  static unsigned fromJson(const QJsonValue& value) { return static_cast<unsigned>(value.toInt()); }
  static unsigned fromVariant(const QVariant& value) { return value.toUInt(); }
  static QJsonValue toJson(unsigned value) { return static_cast<int>(value); }
  static QVariant toVariant(unsigned value) { return value; }
};

template <typename ValueT>
struct TypeTraits<ValueT, typename std::enable_if_t< //
                            isIntWrapper<ValueT> //
                            && (sizeof(typename ValueT::ValueType) <= 4) //
                            >> {
  static ValueT fromJson(const QJsonValue& value) { return ValueT{value.toInt()}; }
  static ValueT fromVariant(const QVariant& value) { return ValueT{value.toInt()}; }
  static QJsonValue toJson(ValueT value) { return static_cast<int>(value.count()); }
  static QVariant toVariant(ValueT value) { return static_cast<int>(value.count()); }
};

template <typename ValueT>
struct TypeTraits<ValueT, typename std::enable_if_t< //
                            isIntWrapper<ValueT> //
                            && (sizeof(typename ValueT::ValueType) > 4) //
                            >> {
  static ValueT fromJson(const QJsonValue& value) { return ValueT{value.toDouble()}; }
  static ValueT fromVariant(const QVariant& value) { return ValueT{value.toLongLong()}; }
  static QJsonValue toJson(ValueT value) { return static_cast<qint64>(value.count()); }
  static QVariant toVariant(ValueT value) { return static_cast<qlonglong>(value.count()); }
};

template <typename ValueT, typename std::enable_if_t<isIntWrapper<ValueT>>* = nullptr>
QDebug operator<<(QDebug debug, ValueT val)
{
  return debug.nospace() << val.count();
}

template <>
struct TypeTraits<QString> {
  static QString fromJson(const QJsonValue& value) { return value.toString(); }
  static QString fromVariant(const QVariant& value) { return value.toString(); }
  static QJsonValue toJson(const QString& value) { return value; }
  static QVariant toVariant(const QString& value) { return value; }
};

template <>
struct TypeTraits<QDate> {
  static QDate fromJson(const QJsonValue& value)
  {
    const auto n = value.toInt();
    const auto y = n / 10000;
    const auto m = (n / 100) % 100;
    const auto d = n % 100;
    return {y, m, d};
  }
  static QDate fromVariant(const QVariant& value) { return value.toDate(); }
  static QJsonValue toJson(const QDate& value)
  {
    return value.year() * 10000 + value.month() * 100 + value.day();
  }
  static QVariant toVariant(const QDate& value) { return value; }
};

template <>
struct TypeTraits<QDateTime> {
  static QDateTime fromJson(const QJsonValue& value)
  {
    return QDateTime::fromMSecsSinceEpoch(value.toDouble());
  }
  static QDateTime fromVariant(const QVariant& value) { return value.toDateTime(); }
  static QJsonValue toJson(const QDateTime& value) { return value.toMSecsSinceEpoch(); }
  static QVariant toVariant(const QDateTime& value) { return value; }
};

template <>
struct TypeTraits<AssetType> {
  static AssetType fromString(const QString& value);
  static AssetType fromJson(const QJsonValue& value) { return fromString(value.toString()); }
  static AssetType fromVariant(const QVariant& value) { return fromString(value.toString()); }
  static QJsonValue toJson(AssetType value) { return enumString(value); }
  static QVariant toVariant(AssetType value) { return enumString(value); }
};

inline QDebug operator<<(QDebug debug, AssetType type)
{
  return debug.nospace() << enumString(type);
}

template <>
struct TypeTraits<Direct> {
  static Direct fromString(const QString& value);
  static Direct fromJson(const QJsonValue& value) { return fromString(value.toString()); }
  static Direct fromVariant(const QVariant& value) { return fromString(value.toString()); }
  static QJsonValue toJson(Direct value) { return enumString(value); }
  static QVariant toVariant(Direct value) { return enumString(value); }
};

inline QDebug operator<<(QDebug debug, Direct direct)
{
  return debug.nospace() << enumString(direct);
}

template <>
struct TypeTraits<LiqInd> {
  static LiqInd fromString(const QString& value);
  static LiqInd fromJson(const QJsonValue& value) { return fromString(value.toString()); }
  static LiqInd fromVariant(const QVariant& value) { return fromString(value.toString()); }
  static QJsonValue toJson(LiqInd value) { return enumString(value); }
  static QVariant toVariant(LiqInd value) { return enumString(value); }
};

inline QDebug operator<<(QDebug debug, LiqInd liqInd)
{
  return debug.nospace() << enumString(liqInd);
}

template <>
struct TypeTraits<Side> {
  static Side fromString(const QString& value);
  static Side fromJson(const QJsonValue& value) { return fromString(value.toString()); }
  static Side fromVariant(const QVariant& value) { return fromString(value.toString()); }
  static QJsonValue toJson(Side value) { return enumString(value); }
  static QVariant toVariant(Side value) { return enumString(value); }
};

inline QDebug operator<<(QDebug debug, Side side)
{
  return debug.nospace() << enumString(side);
}

template <>
struct TypeTraits<State> {
  static State fromString(const QString& value);
  static State fromJson(const QJsonValue& value) { return fromString(value.toString()); }
  static State fromVariant(const QVariant& value) { return fromString(value.toString()); }
  static QJsonValue toJson(State value) { return enumString(value); }
  static QVariant toVariant(State value) { return enumString(value); }
};

inline QDebug operator<<(QDebug debug, State state)
{
  return debug.nospace() << enumString(state);
}

template <typename ValueT>
ValueT fromJson(const QJsonValue& value)
{
  return TypeTraits<ValueT>::fromJson(value);
}

template <typename ValueT>
ValueT fromVariant(const QVariant& value)
{
  return TypeTraits<ValueT>::fromVariant(value);
}

template <typename ValueT>
QJsonValue toJson(const ValueT& value)
{
  return TypeTraits<ValueT>::toJson(value);
}

template <typename ValueT>
QVariant toVariant(const ValueT& value)
{
  return TypeTraits<ValueT>::toVariant(value);
}

} // ui
} // swirly

#endif // SWIRLYUI_TYPES_HPP
