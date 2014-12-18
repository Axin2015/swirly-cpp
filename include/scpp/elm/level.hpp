/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_LEVEL_HPP
#define SCPP_ELM_LEVEL_HPP

#include <sc/elm/types.h>

#include <iostream>

namespace sc {

/**
 * @addtogroup Level
 * @{
 */

class LevelRef {
    ScLevel* impl_;
public:
    explicit
    LevelRef(ScLevel& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScLevel&() const noexcept
    {
        return *impl_;
    }
    ScLevel*
    c_arg() const noexcept
    {
        return impl_;
    }
    ScTicks
    ticks() const noexcept
    {
        return impl_->ticks;
    }
    // Must be greater than zero.
    ScLots
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

} // sc

#endif // SCPP_ELM_LEVEL_HPP
