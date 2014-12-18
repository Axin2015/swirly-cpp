/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_VIEW_HPP
#define SCPP_ELM_VIEW_HPP

#include <scpp/elm/rec.hpp>

namespace sc {

/**
 * @addtogroup View
 * @{
 */

class ViewRef {
    ScView* impl_;
public:
    explicit
    ViewRef(ScView& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScView&() const noexcept
    {
        return *impl_;
    }
    ScView*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(ViewRef rhs) const noexcept
    {
        return key() == rhs.key();
    }
    bool
    operator !=(ViewRef rhs) const noexcept
    {
        return key() != rhs.key();
    }
    ScKey
    key() const noexcept
    {
        return sc_view_key(impl_->contr.rec->id, impl_->settl_day);
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*impl_->contr.rec};
    }
    ScJd
    settl_day() const noexcept
    {
        return impl_->settl_day;
    }
    ScTicks
    bid_ticks(size_t level) const noexcept
    {
        return impl_->bid_ticks[level];
    }
    ScLots
    bid_lots(size_t level) const noexcept
    {
        return impl_->bid_lots[level];
    }
    size_t
    bid_count(size_t level) const noexcept
    {
        return impl_->bid_count[level];
    }
    ScTicks
    offer_ticks(size_t level) const noexcept
    {
        return impl_->offer_ticks[level];
    }
    ScLots
    offer_lots(size_t level) const noexcept
    {
        return impl_->offer_lots[level];
    }
    size_t
    offer_count(size_t level) const noexcept
    {
        return impl_->offer_count[level];
    }
    ScMillis
    created() const noexcept
    {
        return impl_->created;
    }
};

inline std::ostream&
operator <<(std::ostream& os, ViewRef view)
{
    return os << "crec=" << view.crec().mnem()
              << ",settl_day=" << view.settl_day()
              << ",bid_ticks=" << view.bid_ticks(0)
              << ",bid_lots=" << view.bid_lots(0)
              << ",bid_count=" << view.bid_count(0)
              << ",offer_ticks=" << view.offer_ticks(0)
              << ",offer_lots=" << view.offer_lots(0)
              << ",offer_count=" << view.offer_count(0)
              << ",created=" << view.created();
}

/** @} */

} // sc

#endif // SCPP_ELM_VIEW_HPP
