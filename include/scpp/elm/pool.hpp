/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_POOL_HPP
#define SCPP_ELM_POOL_HPP

#include <scpp/ash/except.hpp>

#include <sc/elm/pool.h>

namespace sc {

/**
 * @addtogroup Pool
 * @{
 */

class Pool {
    ScPool impl_;
public:
    ~Pool() noexcept
    {
        if (impl_)
            sc_pool_destroy(impl_);
    }
    constexpr
    Pool(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    explicit
    Pool(size_t capacity)
        : impl_{sc_pool_create(capacity)}
    {
        if (!impl_)
            throw_exception();
    }
    operator ScPool() const noexcept
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
            sc_pool_destroy(impl_);
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
    ScRec*
    alloc_rec()
    {
        ScRec* const rec = sc_pool_alloc_rec(impl_);
        if (!rec)
            throw_exception();
        return rec;
    }
    void
    free_rec(ScRec* rec) noexcept
    {
        sc_pool_free_rec(impl_, rec);
    }
    ScPerm*
    alloc_perm()
    {
        ScPerm* const perm = sc_pool_alloc_perm(impl_);
        if (!perm)
            throw_exception();
        return perm;
    }
    void
    free_perm(ScPerm* perm) noexcept
    {
        sc_pool_free_perm(impl_, perm);
    }
    ScOrder*
    alloc_order()
    {
        ScOrder* const order = sc_pool_alloc_order(impl_);
        if (!order)
            throw_exception();
        return order;
    }
    void
    free_order(ScOrder* order) noexcept
    {
        sc_pool_free_order(impl_, order);
    }
    ScLevel*
    alloc_level()
    {
        ScLevel* const level = sc_pool_alloc_level(impl_);
        if (!level)
            throw_exception();
        return level;
    }
    void
    free_level(ScLevel* level) noexcept
    {
        sc_pool_free_level(impl_, level);
    }
    ScExec*
    alloc_exec()
    {
        ScExec* const exec = sc_pool_alloc_exec(impl_);
        if (!exec)
            throw_exception();
        return exec;
    }
    void
    free_exec(ScExec* exec) noexcept
    {
        sc_pool_free_exec(impl_, exec);
    }
    ScMatch*
    alloc_match()
    {
        ScMatch* const match = sc_pool_alloc_match(impl_);
        if (!match)
            throw_exception();
        return match;
    }
    void
    free_match(ScMatch* match) noexcept
    {
        sc_pool_free_match(impl_, match);
    }
    ScPosn*
    alloc_posn()
    {
        ScPosn* const posn = sc_pool_alloc_posn(impl_);
        if (!posn)
            throw_exception();
        return posn;
    }
    void
    free_posn(ScPosn* posn) noexcept
    {
        sc_pool_free_posn(impl_, posn);
    }
    ScView*
    alloc_view()
    {
        ScView* const view = sc_pool_alloc_view(impl_);
        if (!view)
            throw_exception();
        return view;
    }
    void
    free_view(ScView* view) noexcept
    {
        sc_pool_free_view(impl_, view);
    }
    ScBook*
    alloc_book()
    {
        ScBook* const book = sc_pool_alloc_book(impl_);
        if (!book)
            throw_exception();
        return book;
    }
    void
    free_book(ScBook* book) noexcept
    {
        sc_pool_free_book(impl_, book);
    }
    ScSub*
    alloc_sub()
    {
        ScSub* const sub = sc_pool_alloc_sub(impl_);
        if (!sub)
            throw_exception();
        return sub;
    }
    void
    free_sub(ScSub* sub) noexcept
    {
        sc_pool_free_sub(impl_, sub);
    }
    ScSess*
    alloc_sess()
    {
        ScSess* const sess = sc_pool_alloc_sess(impl_);
        if (!sess)
            throw_exception();
        return sess;
    }
    void
    free_sess(ScSess* sess) noexcept
    {
        sc_pool_free_sess(impl_, sess);
    }
};

/** @} */

} // sc

#endif // SCPP_ELM_POOL_HPP
