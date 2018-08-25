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
#ifndef SWIRLYUI_TYPES_HXX
#define SWIRLYUI_TYPES_HXX

#include <swirly/fin/Conv.hpp>
#include <swirly/fin/IntTypes.hpp>
#include <swirly/fin/Limits.hpp>

#include <swirly/util/Enum.hpp>
#include <swirly/util/Limits.hpp>

#include <QDate>
#include <QDebug>
#include <QJsonValue>

#include <optional>

// This is acceptable in private header that is only visible to and under the control of the Swirly
// build.

namespace swirly {
namespace ui {

constexpr std::size_t MaxExecs{8};

template <typename ValueT, typename EnableT = void>
struct TypeTraits;

template <>
struct TypeTraits<int> {
    static int from_json(const QJsonValue& value) { return value.toInt(); }
    static int from_variant(const QVariant& value) { return value.toInt(); }
    static QJsonValue to_json(int value) { return value; }
    static QVariant to_variant(int value) { return value; }
};

template <>
struct TypeTraits<unsigned> {
    static unsigned from_json(const QJsonValue& value)
    {
        return static_cast<unsigned>(value.toInt());
    }
    static unsigned from_variant(const QVariant& value) { return value.toUInt(); }
    static QJsonValue to_json(unsigned value) { return static_cast<int>(value); }
    static QVariant to_variant(unsigned value) { return value; }
};

template <typename ValueT>
struct TypeTraits<ValueT,
                  typename std::enable_if_t<                       //
                      is_int_wrapper<ValueT>                       //
                      && (sizeof(typename ValueT::ValueType) <= 4) //
                      >> {
    static ValueT from_json(const QJsonValue& value) { return ValueT{value.toInt()}; }
    static ValueT from_variant(const QVariant& value) { return ValueT{value.toInt()}; }
    static QJsonValue to_json(ValueT value) { return static_cast<int>(value.count()); }
    static QVariant to_variant(ValueT value) { return static_cast<int>(value.count()); }
};

template <typename ValueT>
struct TypeTraits<ValueT,
                  typename std::enable_if_t<                      //
                      is_int_wrapper<ValueT>                      //
                      && (sizeof(typename ValueT::ValueType) > 4) //
                      >> {
    static ValueT from_json(const QJsonValue& value) { return ValueT{value.toDouble()}; }
    static ValueT from_variant(const QVariant& value) { return ValueT{value.toLongLong()}; }
    static QJsonValue to_json(ValueT value) { return static_cast<qint64>(value.count()); }
    static QVariant to_variant(ValueT value) { return static_cast<qlonglong>(value.count()); }
};

template <typename ValueT, typename std::enable_if_t<is_int_wrapper<ValueT>>* = nullptr>
QDebug operator<<(QDebug debug, ValueT val)
{
    return debug.nospace() << val.count();
}

template <>
struct TypeTraits<QString> {
    static QString from_json(const QJsonValue& value) { return value.toString(); }
    static QString from_variant(const QVariant& value) { return value.toString(); }
    static QJsonValue to_json(const QString& value) { return value; }
    static QVariant to_variant(const QString& value) { return value; }
};

inline auto date_to_iso(const QDate& value) noexcept
{
    return value.year() * 10000 + value.month() * 100 + value.day();
}

template <>
struct TypeTraits<QDate> {
    static QDate from_json(const QJsonValue& value)
    {
        const auto n = value.toInt();
        const auto y = n / 10000;
        const auto m = (n / 100) % 100;
        const auto d = n % 100;
        return {y, m, d};
    }
    static QDate from_variant(const QVariant& value) { return value.toDate(); }
    static QJsonValue to_json(const QDate& value) { return date_to_iso(value); }
    static QVariant to_variant(const QDate& value) { return value; }
};

template <>
struct TypeTraits<QDateTime> {
    static QDateTime from_json(const QJsonValue& value)
    {
        return QDateTime::fromMSecsSinceEpoch(value.toDouble());
    }
    static QDateTime from_variant(const QVariant& value) { return value.toDateTime(); }
    static QJsonValue to_json(const QDateTime& value) { return value.toMSecsSinceEpoch(); }
    static QVariant to_variant(const QDateTime& value) { return value; }
};

template <>
struct TypeTraits<AssetType> {
    static AssetType from_string(const QString& value);
    static AssetType from_json(const QJsonValue& value) { return from_string(value.toString()); }
    static AssetType from_variant(const QVariant& value) { return from_string(value.toString()); }
    static QJsonValue to_json(AssetType value) { return enum_string(value); }
    static QVariant to_variant(AssetType value) { return enum_string(value); }
};

inline QDebug operator<<(QDebug debug, AssetType type)
{
    return debug.nospace() << enum_string(type);
}

template <>
struct TypeTraits<Direct> {
    static Direct from_string(const QString& value);
    static Direct from_json(const QJsonValue& value) { return from_string(value.toString()); }
    static Direct from_variant(const QVariant& value) { return from_string(value.toString()); }
    static QJsonValue to_json(Direct value) { return enum_string(value); }
    static QVariant to_variant(Direct value) { return enum_string(value); }
};

inline QDebug operator<<(QDebug debug, Direct direct)
{
    return debug.nospace() << enum_string(direct);
}

template <>
struct TypeTraits<LiqInd> {
    static LiqInd from_string(const QString& value);
    static LiqInd from_json(const QJsonValue& value) { return from_string(value.toString()); }
    static LiqInd from_variant(const QVariant& value) { return from_string(value.toString()); }
    static QJsonValue to_json(LiqInd value) { return enum_string(value); }
    static QVariant to_variant(LiqInd value) { return enum_string(value); }
};

inline QDebug operator<<(QDebug debug, LiqInd liq_ind)
{
    return debug.nospace() << enum_string(liq_ind);
}

template <>
struct TypeTraits<Side> {
    static Side from_string(const QString& value);
    static Side from_json(const QJsonValue& value) { return from_string(value.toString()); }
    static Side from_variant(const QVariant& value) { return from_string(value.toString()); }
    static QJsonValue to_json(Side value) { return enum_string(value); }
    static QVariant to_variant(Side value) { return enum_string(value); }
};

inline QDebug operator<<(QDebug debug, Side side)
{
    return debug.nospace() << enum_string(side);
}

template <>
struct TypeTraits<State> {
    static State from_string(const QString& value);
    static State from_json(const QJsonValue& value) { return from_string(value.toString()); }
    static State from_variant(const QVariant& value) { return from_string(value.toString()); }
    static QJsonValue to_json(State value) { return enum_string(value); }
    static QVariant to_variant(State value) { return enum_string(value); }
};

inline QDebug operator<<(QDebug debug, State state)
{
    return debug.nospace() << enum_string(state);
}

template <typename ValueT>
ValueT from_json(const QJsonValue& value)
{
    return TypeTraits<ValueT>::from_json(value);
}

template <typename ValueT>
ValueT from_variant(const QVariant& value)
{
    return TypeTraits<ValueT>::from_variant(value);
}

template <typename ValueT>
QJsonValue to_json(const ValueT& value)
{
    return TypeTraits<ValueT>::to_json(value);
}

template <typename ValueT>
QVariant to_variant(const ValueT& value)
{
    return TypeTraits<ValueT>::to_variant(value);
}

using ExecKey = std::pair<Id64, Id64>;
using ExecKeys = std::vector<ExecKey>;

using OrderKey = std::pair<Id64, Id64>;
using OrderKeys = std::vector<OrderKey>;

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_TYPES_HXX
