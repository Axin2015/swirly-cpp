/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_BOOK_HPP
#define SCPP_FIG_BOOK_HPP

#include <scpp/elm/rec.hpp>
#include <scpp/elm/side.hpp>

#include <sc/fig/book.h>

#include <iostream>

namespace sc {

/**
 * @addtogroup Book
 * @{
 */

class Book {
    mutable ScBook impl_;
public:
    ~Book() noexcept
    {
        sc_book_term(&impl_);
    }
    explicit
    Book(ScRec& crec, ScJd settl_day, ScPool pool) noexcept
    {
        sc_book_init(&impl_, &crec, settl_day, pool);
    }
    operator ScBook&() noexcept
    {
        return impl_;
    }
    ScBook*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Book(const Book&) = delete;

    Book&
    operator =(const Book&) = delete;

    bool
    operator ==(const Book& rhs) const noexcept
    {
        return key() == rhs.key();
    }
    bool
    operator !=(const Book& rhs) const noexcept
    {
        return key() != rhs.key();
    }
    ScKey
    key() const noexcept
    {
        return sc_book_key(impl_.crec->id, impl_.settl_day);
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*sc_book_crec(&impl_)};
    }
    ScJd
    settl_day() const noexcept
    {
        return sc_book_settl_day(&impl_);
    }
    SideRef
    bid_side() const noexcept
    {
        return SideRef{*sc_book_bid_side(&impl_)};
    }
    SideRef
    offer_side() const noexcept
    {
        return SideRef{*sc_book_offer_side(&impl_)};
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Book& book)
{
    return os << "key=" << book.key()
              << ",crec=" << book.crec().mnem()
              << ",settl_day" << book.settl_day();
}

class BookRef {
    ScBook* impl_;
public:
    explicit
    BookRef(ScBook& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScBook&() const noexcept
    {
        return *impl_;
    }
    ScBook*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(BookRef rhs) const noexcept
    {
        return key() == rhs.key();
    }
    bool
    operator !=(BookRef rhs) const noexcept
    {
        return key() != rhs.key();
    }
    ScKey
    key() const noexcept
    {
        return sc_book_key(impl_->crec->id, impl_->settl_day);
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*sc_book_crec(impl_)};
    }
    ScJd
    settl_day() const noexcept
    {
        return sc_book_settl_day(impl_);
    }
    SideRef
    bid_side() const noexcept
    {
        return SideRef{*sc_book_bid_side(impl_)};
    }
    SideRef
    offer_side() const noexcept
    {
        return SideRef{*sc_book_offer_side(impl_)};
    }
};

inline std::ostream&
operator <<(std::ostream& os, BookRef book)
{
    return os << "key=" << book.key()
              << ",crec=" << book.crec().mnem()
              << ",settl_day" << book.settl_day();
}

/** @} */

} // sc

#endif // SCPP_FIG_BOOK_HPP
