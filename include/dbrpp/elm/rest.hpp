/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_REST_HPP
#define DBRPP_ELM_REST_HPP

#include <dbrpp/elm/types.hpp>

#include <dbrpp/ash/except.hpp>

#include <dbr/elm/rest.h>

namespace dbr {

/**
 * @addtogroup Rest
 * @{
 */

class Rest {
    DbrRest impl_;
public:
    Rest() noexcept
    {
        dbr_rest_init(&impl_);
    }
    void
    reset() noexcept
    {
        dbr_rest_init(&impl_);
    }
    bool
    rurl(const char* buf, size_t size)
    {
        const int ret = dbr_rest_rurl(&impl_, buf, size);
        if (ret < 0)
            throw_exception();
        return ret;
    }
    bool
    json(const char* buf, size_t size)
    {
        const int ret = dbr_rest_json(&impl_, buf, size);
        if (ret < 0)
            throw_exception();
        return ret;
    }
    unsigned
    fields() const noexcept
    {
        return impl_.fields;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    StringRef
    accnt() const noexcept
    {
        return StringRef{impl_.accnt, MaxSize{DBR_MNEM_MAX}};
    }
    StringRef
    giveup() const noexcept
    {
        return StringRef{impl_.giveup, MaxSize{DBR_MNEM_MAX}};
    }
    StringRef
    contr() const noexcept
    {
        return StringRef{impl_.contr, MaxSize{DBR_MNEM_MAX}};
    }
    DbrIsoDate
    settl_date() const noexcept
    {
        return impl_.settl_date;
    }
    StringRef
    ref() const noexcept
    {
        return StringRef{impl_.ref, MaxSize{DBR_REF_MAX}};
    }
    int
    action() const noexcept
    {
        return impl_.action;
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
    DbrLots
    min_lots() const noexcept
    {
        return impl_.min_lots;
    }
};

/** @} */

} // dbr

#endif // DBRPP_ELM_REST_HPP
