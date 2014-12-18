/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_LEVEL_HPP
#define DBRPP_ELM_LEVEL_HPP

#include <dbr/elm/types.h>

#include <iostream>

namespace dbr {

/**
 * @addtogroup Level
 * @{
 */

class LevelRef {
    DbrLevel* impl_;
public:
    explicit
    LevelRef(DbrLevel& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrLevel&() const noexcept
    {
        return *impl_;
    }
    DbrLevel*
    c_arg() const noexcept
    {
        return impl_;
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_->ticks;
    }
    // Must be greater than zero.
    DbrLots
    lots() const noexcept
    {
        return impl_->lots;
    }
    size_t
    count() const noexcept
    {
        return impl_->count;
    }
};

inline std::ostream&
operator <<(std::ostream& os, LevelRef level)
{
    return os << "ticks=" << level.ticks()
              << ",lots=" << level.lots()
              << ",count=" << level.count();
}

/** @} */

} // dbr

#endif // DBRPP_ELM_LEVEL_HPP
