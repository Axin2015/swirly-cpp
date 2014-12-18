/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_PERM_HPP
#define SCPP_ELM_PERM_HPP

#include <scpp/elm/rec.hpp>

namespace sc {

/**
 * @addtogroup Perm
 * @{
 */

class PermRef {
    ScPerm* impl_;
public:
    explicit
    PermRef(ScPerm& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScPerm&() const noexcept
    {
        return *impl_;
    }
    ScPerm*
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

} // sc

#endif // SCPP_ELM_PERM_HPP
