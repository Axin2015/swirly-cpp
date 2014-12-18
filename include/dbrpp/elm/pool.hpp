/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_POOL_HPP
#define DBRPP_ELM_POOL_HPP

#include <dbrpp/ash/except.hpp>

#include <dbr/elm/pool.h>

namespace dbr {

/**
 * @addtogroup Pool
 * @{
 */

class Pool {
    DbrPool impl_;
public:
    ~Pool() noexcept
    {
        if (impl_)
            dbr_pool_destroy(impl_);
    }
    constexpr
    Pool(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    explicit
    Pool(size_t capacity)
        : impl_{dbr_pool_create(capacity)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrPool() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Pool(const Pool&) = delete;

    Pool&
    operator =(const Pool&) = delete;

    // Move semantics.

    Pool(Pool&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Pool&
    operator =(Pool&& rhs) noexcept
    {
        if (impl_) {
            dbr_pool_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Pool& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    DbrRec*
    alloc_rec()
    {
        DbrRec* const rec = dbr_pool_alloc_rec(impl_);
        if (!rec)
            throw_exception();
        return rec;
    }
    void
    free_rec(DbrRec* rec) noexcept
    {
        dbr_pool_free_rec(impl_, rec);
    }
    DbrPerm*
    alloc_perm()
    {
        DbrPerm* const perm = dbr_pool_alloc_perm(impl_);
        if (!perm)
            throw_exception();
        return perm;
    }
    void
    free_perm(DbrPerm* perm) noexcept
    {
        dbr_pool_free_perm(impl_, perm);
    }
    DbrOrder*
    alloc_order()
    {
        DbrOrder* const order = dbr_pool_alloc_order(impl_);
        if (!order)
            throw_exception();
        return order;
    }
    void
    free_order(DbrOrder* order) noexcept
    {
        dbr_pool_free_order(impl_, order);
    }
    DbrLevel*
    alloc_level()
    {
        DbrLevel* const level = dbr_pool_alloc_level(impl_);
        if (!level)
            throw_exception();
        return level;
    }
    void
    free_level(DbrLevel* level) noexcept
    {
        dbr_pool_free_level(impl_, level);
    }
    DbrExec*
    alloc_exec()
    {
        DbrExec* const exec = dbr_pool_alloc_exec(impl_);
        if (!exec)
            throw_exception();
        return exec;
    }
    void
    free_exec(DbrExec* exec) noexcept
    {
        dbr_pool_free_exec(impl_, exec);
    }
    DbrMatch*
    alloc_match()
    {
        DbrMatch* const match = dbr_pool_alloc_match(impl_);
        if (!match)
            throw_exception();
        return match;
    }
    void
    free_match(DbrMatch* match) noexcept
    {
        dbr_pool_free_match(impl_, match);
    }
    DbrPosn*
    alloc_posn()
    {
        DbrPosn* const posn = dbr_pool_alloc_posn(impl_);
        if (!posn)
            throw_exception();
        return posn;
    }
    void
    free_posn(DbrPosn* posn) noexcept
    {
        dbr_pool_free_posn(impl_, posn);
    }
    DbrView*
    alloc_view()
    {
        DbrView* const view = dbr_pool_alloc_view(impl_);
        if (!view)
            throw_exception();
        return view;
    }
    void
    free_view(DbrView* view) noexcept
    {
        dbr_pool_free_view(impl_, view);
    }
    DbrBook*
    alloc_book()
    {
        DbrBook* const book = dbr_pool_alloc_book(impl_);
        if (!book)
            throw_exception();
        return book;
    }
    void
    free_book(DbrBook* book) noexcept
    {
        dbr_pool_free_book(impl_, book);
    }
    DbrSub*
    alloc_sub()
    {
        DbrSub* const sub = dbr_pool_alloc_sub(impl_);
        if (!sub)
            throw_exception();
        return sub;
    }
    void
    free_sub(DbrSub* sub) noexcept
    {
        dbr_pool_free_sub(impl_, sub);
    }
    DbrSess*
    alloc_sess()
    {
        DbrSess* const sess = dbr_pool_alloc_sess(impl_);
        if (!sess)
            throw_exception();
        return sess;
    }
    void
    free_sess(DbrSess* sess) noexcept
    {
        dbr_pool_free_sess(impl_, sess);
    }
};

/** @} */

} // dbr

#endif // DBRPP_ELM_POOL_HPP
