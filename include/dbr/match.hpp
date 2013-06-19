#ifndef DBR_MATCH_HPP
#define DBR_MATCH_HPP

#include <dbr/order.hpp>
#include <dbr/posn.hpp>
#include <dbr/trade.hpp>

namespace dbr {

class Match {
    const DbrMatch& impl_;
public:
    explicit
    Match(const DbrMatch& impl) noexcept
        : impl_(impl)
    {
    }
    bool
    operator ==(Match rhs) const noexcept
    {
        return impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(Match rhs) const noexcept
    {
        return impl_.id != rhs.impl_.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    Order
    maker_order() const noexcept
    {
        return Order(*impl_.maker_order);
    }
    Posn
    maker_posn() const noexcept
    {
        return Posn(*impl_.maker_posn);
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_.ticks;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_.lots;
    }
    Trade
    taker_trade() const noexcept
    {
        return Trade(*impl_.taker_trade);
    }
    Trade
    maker_trade() const noexcept
    {
        return Trade(*impl_.maker_trade);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Match match)
{
    return os << "id=" << match.id()
              << ",maker_order=" << match.maker_order().id()
              << ",maker_posn=" << match.maker_posn().id()
              << ",ticks=" << match.ticks()
              << ",lots=" << match.lots()
              << ",taker_trade=" << match.taker_trade().id()
              << ",maker_trade=" << match.maker_trade().id();
}
} // dbr

#endif // DBR_MATCH_HPP
