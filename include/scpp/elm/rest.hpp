/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_REST_HPP
#define SCPP_ELM_REST_HPP

#include <scpp/elm/types.hpp>

#include <scpp/ash/except.hpp>

#include <sc/elm/rest.h>

namespace sc {

/**
 * @addtogroup Rest
 * @{
 */

class Rest {
    ScRest impl_;
public:
    Rest() noexcept
    {
        sc_rest_init(&impl_);
    }
    void
    reset() noexcept
    {
        sc_rest_init(&impl_);
    }
    bool
    rurl(const char* buf, size_t size)
    {
        const int ret = sc_rest_rurl(&impl_, buf, size);
        if (ret < 0)
            throw_exception();
        return ret;
    }
    bool
    json(const char* buf, size_t size)
    {
        const int ret = sc_rest_json(&impl_, buf, size);
        if (ret < 0)
            throw_exception();
        return ret;
    }
    unsigned
    fields() const noexcept
    {
        return impl_.fields;
    }
    ScIden
    id() const noexcept
    {
        return impl_.id;
    }
    StringRef
    accnt() const noexcept
    {
        return StringRef{impl_.accnt, MaxSize{SC_MNEM_MAX}};
    }
    StringRef
    giveup() const noexcept
    {
        return StringRef{impl_.giveup, MaxSize{SC_MNEM_MAX}};
    }
    StringRef
    contr() const noexcept
    {
        return StringRef{impl_.contr, MaxSize{SC_MNEM_MAX}};
    }
    ScIsoDate
    settl_date() const noexcept
    {
        return impl_.settl_date;
    }
    StringRef
    ref() const noexcept
    {
        return StringRef{impl_.ref, MaxSize{SC_REF_MAX}};
    }
    int
    action() const noexcept
    {
        return impl_.action;
    }
    ScTicks
    ticks() const noexcept
    {
        return impl_.ticks;
    }
    ScLots
    lots() const noexcept
    {
        return impl_.lots;
    }
    ScLots
    min_lots() const noexcept
    {
        return impl_.min_lots;
    }
};

/** @} */

} // sc

#endif // SCPP_ELM_REST_HPP
