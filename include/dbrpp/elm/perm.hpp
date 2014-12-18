/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_PERM_HPP
#define DBRPP_ELM_PERM_HPP

#include <dbrpp/elm/rec.hpp>

namespace dbr {

/**
 * @addtogroup Perm
 * @{
 */

class PermRef {
    DbrPerm* impl_;
public:
    explicit
    PermRef(DbrPerm& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrPerm&() const noexcept
    {
        return *impl_;
    }
    DbrPerm*
    c_arg() noexcept
    {
        return impl_;
    }
    AccntRecRef
    trec() const noexcept
    {
        return AccntRecRef{*impl_->trader.rec};
    }
    AccntRecRef
    grec() const noexcept
    {
        return AccntRecRef{*impl_->giveup.rec};
    }
};

inline std::ostream&
operator <<(std::ostream& os, PermRef perm)
{
    return os << "trader=" << perm.trec().mnem()
              << ",giveup=" << perm.grec().mnem();
}

/** @} */

} // dbr

#endif // DBRPP_ELM_PERM_HPP
