/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIX_LIST_HPP
#define SWIRLY_FIX_LIST_HPP

#include <swirly/fix/Traits.hpp>

namespace swirly {
inline namespace fix {

struct NoTagOpts {
    template <typename ListT, typename TagT>
    static constexpr bool empty(const ListT&, TagT)
    {
        return false;
    }
};

template <typename TagOptsT, Tag...>
class TagListView;

template <typename TagOptsT>
class TagListView<TagOptsT> {
  public:
    void get() const noexcept;

    template <typename StreamT>
    void put(StreamT& os) const noexcept
    {
    }
    void clear() noexcept {}
    bool set(Tag tag, std::string_view sv) noexcept { return false; }
};

template <typename TagOptsT, Tag TagN, Tag... TailN>
class TagListView<TagOptsT, TagN, TailN...> : TagListView<TagOptsT, TailN...> {
  public:
    using TagType = fix::TagType<TagN>;
    using TagTraits = fix::TagTraits<TagN>;
    using ValueType = typename TagTraits::ValueType;
    using ViewType = typename TagTraits::ViewType;

    template <template <typename, Tag...> typename TemplT>
    using Rebind = TemplT<TagOptsT, TagN, TailN...>;

    using TagListView<TagOptsT, TailN...>::get;
    const auto& get(TagType) const noexcept { return value; }
    auto& get(TagType) noexcept { return value; }
    void put(FixStream& os) const
    {
        if (!TagOptsT::empty(*this, TagType{})) {
            os.put_num(+TagN);
            os.put('=');
            TagTraits::put(os, value);
            os.put('\1');
        }
        TagListView<TagOptsT, TailN...>::put(os);
    }
    void put(std::ostream& os) const
    {
        if (!TagOptsT::empty(*this, TagType{})) {
            os << +TagN;
            os.put('=');
            TagTraits::put(os, value);
            os.put('\1');
        }
        TagListView<TagOptsT, TailN...>::put(os);
    }
    void clear()
    {
        TagListView<TagOptsT, TailN...>::clear();
        TagTraits::clear(value);
    }
    void set(TagType, std::string_view sv) { TagTraits::parse(sv, value); }
    bool set(Tag tag, std::string_view sv)
    {
        if (tag == TagN) {
            TagTraits::parse(sv, value);
            return true;
        }
        return TagListView<TagOptsT, TailN...>::set(tag, sv);
    }

  private:
    ViewType value{};
};

template <Tag GetN, typename TagOptsT, Tag... TagN>
const auto& get(const TagListView<TagOptsT, TagN...>& tl) noexcept
{
    return tl.get(TagType<GetN>{});
}

template <Tag GetN, typename TagOptsT, Tag... TagN>
auto& get(TagListView<TagOptsT, TagN...>& tl) noexcept
{
    return tl.get(TagType<GetN>{});
}

template <Tag GetN, typename TagOptsT, Tag... TagN>
void set(TagListView<TagOptsT, TagN...>& tl, std::string_view sv)
{
    tl.set(TagType<GetN>{}, sv);
}

template <typename StreamT, typename TagOptsT, Tag... TagN>
StreamT& operator<<(StreamT& os, const TagListView<TagOptsT, TagN...>& tl)
{
    tl.put(os);
    return os;
}

template <typename TagOptsT, Tag...>
class TagList;

template <typename TagOptsT>
class TagList<TagOptsT> {
  public:
    void get() const noexcept;

    template <typename StreamT>
    void put(StreamT& os) const noexcept
    {
    }
    void clear() noexcept {}
    bool set(Tag tag, std::string_view sv) noexcept { return false; }
};

template <typename TagOptsT, Tag TagN, Tag... TailN>
class TagList<TagOptsT, TagN, TailN...> : TagList<TagOptsT, TailN...> {
  public:
    using TagType = fix::TagType<TagN>;
    using TagTraits = fix::TagTraits<TagN>;
    using ValueType = typename TagTraits::ValueType;
    using ViewType = typename TagTraits::ViewType;

    template <template <typename, Tag...> typename TemplT>
    using Rebind = TemplT<TagOptsT, TagN, TailN...>;
    using View = Rebind<TagListView>;

    using TagList<TagOptsT, TailN...>::get;
    const auto& get(TagType) const noexcept { return value; }
    auto& get(TagType) noexcept { return value; }
    void put(FixStream& os) const
    {
        if (!TagOptsT::empty(*this, TagType{})) {
            os.put_num(+TagN);
            os.put('=');
            TagTraits::put(os, value);
            os.put('\1');
        }
        TagList<TagOptsT, TailN...>::put(os);
    }
    void put(std::ostream& os) const
    {
        if (!TagOptsT::empty(*this, TagType{})) {
            os << +TagN;
            os.put('=');
            TagTraits::put(os, value);
            os.put('\1');
        }
        TagList<TagOptsT, TailN...>::put(os);
    }
    void clear()
    {
        TagList<TagOptsT, TailN...>::clear();
        TagTraits::clear(value);
    }
    void set(TagType, std::string_view sv) { TagTraits::parse(sv, value); }
    bool set(Tag tag, std::string_view sv)
    {
        if (tag == TagN) {
            TagTraits::parse(sv, value);
            return true;
        }
        return TagList<TagOptsT, TailN...>::set(tag, sv);
    }

  private:
    ValueType value{};
};

template <Tag GetN, typename TagOptsT, Tag... TagN>
const auto& get(const TagList<TagOptsT, TagN...>& tl) noexcept
{
    return tl.get(TagType<GetN>{});
}

template <Tag GetN, typename TagOptsT, Tag... TagN>
auto& get(TagList<TagOptsT, TagN...>& tl) noexcept
{
    return tl.get(TagType<GetN>{});
}

template <Tag GetN, typename TagOptsT, Tag... TagN>
void set(TagList<TagOptsT, TagN...>& tl, std::string_view sv)
{
    tl.set(TagType<GetN>{}, sv);
}

template <typename StreamT, typename TagOptsT, Tag... TagN>
StreamT& operator<<(StreamT& os, const TagList<TagOptsT, TagN...>& tl)
{
    tl.put(os);
    return os;
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_LIST_HPP
