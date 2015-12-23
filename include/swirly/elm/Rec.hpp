/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_ELM_REC_HPP
#define SWIRLY_ELM_REC_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Compare.hpp>

#include <boost/intrusive/set.hpp>

#include <memory> // unique_ptr<>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Rec : public Comparable<Rec> {
protected:
    const RecType type_;
    const Mnem mnem_;
    Display display_;
public:
    boost::intrusive::set_member_hook<> set_hook_;

    Rec(RecType type, const char* mnem, const char* display) noexcept
    :   type_{type},
        mnem_{mnem},
        display_{display}
    {
    }

    virtual ~Rec() noexcept;

    // Copy.
    Rec(const Rec&) = default;
    Rec& operator =(const Rec&) = default;

    // Move.
    Rec(Rec&&) = default;
    Rec& operator =(Rec&&) = default;

    int compare(const Rec& rhs) const noexcept
    {
        int ret {swirly::compare(type_, rhs.type_)};
        if (ret == 0)
            ret = mnem_.compare(rhs.mnem_);
        return ret;
    }
    RecType recType() const noexcept
    {
        return type_;
    }
    StringView mnem() const noexcept
    {
        return mnem_;
    }
    StringView display() const noexcept
    {
        return display_;
    }
};

class SWIRLY_API RecSet {
    using Option = boost::intrusive::member_hook<Rec, boost::intrusive::set_member_hook<>, &Rec::set_hook_>;
    using Set = boost::intrusive::set<Rec, Option>;
    Set set_;
public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    RecSet() = default;

    ~RecSet() noexcept
    {
        set_.clear_and_dispose([](Rec* ptr) { delete ptr; });
    }

    // Copy.
    RecSet(const RecSet&) = delete;
    RecSet& operator =(const RecSet&) = delete;

    // Move.
    RecSet(RecSet&&) = default;
    RecSet& operator =(RecSet&&) = default;

    bool insert(std::unique_ptr<Rec> rec) noexcept
    {
        auto ret = set_.insert(*rec);
        if (ret.second) {
            // Take ownership if inserted.
            rec.release();
        }
        return ret.second;
    }

    void insertOrReplace(std::unique_ptr<Rec> rec) noexcept
    {
        auto ret = set_.insert(*rec);
        if (!ret.second) {
            // Replace if exists.
            set_.replace_node(ret.first, *rec);
        }
        rec.release();
    }

    // Begin.
    Iterator begin() noexcept
    {
        return set_.begin();
    }
    ConstIterator begin() const noexcept
    {
        return set_.begin();
    }
    ConstIterator cbegin() const noexcept
    {
        return set_.cbegin();
    }

    // End.
    Iterator end() noexcept
    {
        return set_.end();
    }
    ConstIterator end() const noexcept
    {
        return set_.end();
    }
    ConstIterator cend() const noexcept
    {
        return set_.cend();
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_REC_HPP
