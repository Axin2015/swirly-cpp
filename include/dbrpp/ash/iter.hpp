/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_ITER_HPP
#define DBRPP_ASH_ITER_HPP

#include <iterator>

namespace dbr {

/**
 * @addtogroup Iter
 * @{
 */

template <class PolicyT>
class ConstBiDirectionalIterator;

template <class PolicyT>
class BiDirectionalIterator {
    friend class ConstBiDirectionalIterator<PolicyT>;
public:
    typedef std::forward_iterator_tag IteratorCategory;
    typedef std::ptrdiff_t DifferenceType;
    typedef typename PolicyT::Node Node;
    typedef typename PolicyT::Entry ValueType;
    typedef typename PolicyT::Entry* Pointer;
    typedef typename PolicyT::Entry& Reference;

private:
    Node* node_;

public:
    // Standard typedefs.

    typedef IteratorCategory iterator_category;
    typedef DifferenceType difference_type;
    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;

    BiDirectionalIterator(Node* node = nullptr) noexcept
        : node_{node}
    {
    }
    Reference
    operator *() const noexcept
    {
        return *PolicyT::entry(node_);
    }
    Pointer
    operator ->() const noexcept
    {
        return PolicyT::entry(node_);
    }
    BiDirectionalIterator&
    operator ++() noexcept
    {
        node_ = PolicyT::next(node_);
        return *this;
    }
    BiDirectionalIterator
    operator ++(int) noexcept
    {
        BiDirectionalIterator tmp{*this};
        node_ = PolicyT::next(node_);
        return tmp;
    }
    BiDirectionalIterator&
    operator --() noexcept
    {
        node_ = PolicyT::prev(node_);
        return *this;
    }
    BiDirectionalIterator
    operator --(int) noexcept
    {
        BiDirectionalIterator tmp{*this};
        node_ = PolicyT::prev(node_);
        return tmp;
    }
    bool
    operator ==(BiDirectionalIterator rhs) const noexcept
    {
        return node_ == rhs.node_;
    }
    bool
    operator !=(BiDirectionalIterator rhs) const noexcept
    {
        return node_ != rhs.node_;
    }
};

template <class PolicyT>
class ConstBiDirectionalIterator {
public:
    typedef std::forward_iterator_tag IteratorCategory;
    typedef std::ptrdiff_t DifferenceType;
    typedef typename PolicyT::Node Node;
    typedef typename PolicyT::Entry ValueType;
    typedef /*const*/ typename PolicyT::Entry* Pointer;
    typedef /*const*/ typename PolicyT::Entry& Reference;

private:
    /*const*/ Node* node_;

public:
    // Standard typedefs.

    typedef IteratorCategory iterator_category;
    typedef DifferenceType difference_type;
    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;

    ConstBiDirectionalIterator(/*const*/ Node* node = nullptr) noexcept
        : node_{node}
    {
    }
    ConstBiDirectionalIterator(BiDirectionalIterator<PolicyT> it) noexcept
        : node_{it.node_}
    {
    }
    Reference
    operator *() const noexcept
    {
        return *PolicyT::entry(node_);
    }
    Pointer
    operator ->() const noexcept
    {
        return PolicyT::entry(node_);
    }
    ConstBiDirectionalIterator&
    operator ++() noexcept
    {
        node_ = PolicyT::next(node_);
        return *this;
    }
    ConstBiDirectionalIterator
    operator ++(int) noexcept
    {
        ConstBiDirectionalIterator tmp{*this};
        node_ = PolicyT::next(node_);
        return tmp;
    }
    bool
    operator ==(ConstBiDirectionalIterator rhs) const noexcept
    {
        return node_ == rhs.node_;
    }
    bool
    operator !=(ConstBiDirectionalIterator rhs) const noexcept
    {
        return node_ != rhs.node_;
    }
};

template <typename PolicyT>
inline bool
operator ==(BiDirectionalIterator<PolicyT> lhs,
            ConstBiDirectionalIterator<PolicyT> rhs) noexcept
{
    return ConstBiDirectionalIterator<PolicyT>{lhs} == rhs;
}

template <typename PolicyT>
inline bool
operator ==(ConstBiDirectionalIterator<PolicyT> lhs,
            BiDirectionalIterator<PolicyT> rhs) noexcept
{
    return lhs == ConstBiDirectionalIterator<PolicyT>{rhs};
}

template <typename PolicyT>
inline bool
operator !=(BiDirectionalIterator<PolicyT> lhs,
            ConstBiDirectionalIterator<PolicyT> rhs) noexcept
{
    return ConstBiDirectionalIterator<PolicyT>{lhs} != rhs;
}

template <typename PolicyT>
inline bool
operator !=(ConstBiDirectionalIterator<PolicyT> lhs,
            BiDirectionalIterator<PolicyT> rhs) noexcept
{
    return lhs != ConstBiDirectionalIterator<PolicyT>{rhs};
}

template <typename PolicyT>
struct ReverseBiDirectionalPolicy {
    typedef typename PolicyT::Node Node;
    typedef typename PolicyT::Entry Entry;
    static Node*
    next(Node* node) noexcept
    {
        return PolicyT::prev(node);
    }
    static const Node*
    next(const Node* node) noexcept
    {
        return PolicyT::prev(node);
    }
    static Node*
    prev(Node* node) noexcept
    {
        return PolicyT::next(node);
    }
    static const Node*
    prev(const Node* node) noexcept
    {
        return PolicyT::next(node);
    }
    static Entry*
    entry(Node* node)
    {
        return PolicyT::entry(node);
    }
    static const Entry*
    entry(const Node* node)
    {
        return PolicyT::entry(node);
    }
};

template <typename PolicyT>
using ReverseBiDirectionalIterator = BiDirectionalIterator<ReverseBiDirectionalPolicy<PolicyT>>;

template <typename PolicyT>
using ConstReverseBiDirectionalIterator = ConstBiDirectionalIterator<
    ReverseBiDirectionalPolicy<PolicyT>>;

template <class PolicyT>
class ConstForwardIterator;

template <class PolicyT>
class ForwardIterator {
    friend class ConstForwardIterator<PolicyT>;
public:
    typedef std::forward_iterator_tag IteratorCategory;
    typedef std::ptrdiff_t DifferenceType;
    typedef typename PolicyT::Node Node;
    typedef typename PolicyT::Entry ValueType;
    typedef typename PolicyT::Entry* Pointer;
    typedef typename PolicyT::Entry& Reference;

private:
    Node* node_;

public:
    // Standard typedefs.

    typedef IteratorCategory iterator_category;
    typedef DifferenceType difference_type;
    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;

    ForwardIterator(Node* node = nullptr) noexcept
        : node_{node}
    {
    }
    Reference
    operator *() const noexcept
    {
        return *PolicyT::entry(node_);
    }
    Pointer
    operator ->() const noexcept
    {
        return PolicyT::entry(node_);
    }
    ForwardIterator&
    operator ++() noexcept
    {
        node_ = PolicyT::next(node_);
        return *this;
    }
    ForwardIterator
    operator ++(int) noexcept
    {
        ForwardIterator tmp{*this};
        node_ = PolicyT::next(node_);
        return tmp;
    }
    bool
    operator ==(ForwardIterator rhs) const noexcept
    {
        return node_ == rhs.node_;
    }
    bool
    operator !=(ForwardIterator rhs) const noexcept
    {
        return node_ != rhs.node_;
    }
};

template <class PolicyT>
class ConstForwardIterator {
public:
    typedef std::forward_iterator_tag IteratorCategory;
    typedef std::ptrdiff_t DifferenceType;
    typedef typename PolicyT::Node Node;
    typedef typename PolicyT::Entry ValueType;
    typedef /*const*/ typename PolicyT::Entry* Pointer;
    typedef /*const*/ typename PolicyT::Entry& Reference;

private:
    /*const*/ Node* node_;

public:
    // Standard typedefs.

    typedef IteratorCategory iterator_category;
    typedef DifferenceType difference_type;
    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;

    ConstForwardIterator(/*const*/ Node* node = nullptr) noexcept
        : node_{node}
    {
    }
    ConstForwardIterator(ForwardIterator<PolicyT> it) noexcept
        : node_{it.node_}
    {
    }
    Reference
    operator *() const noexcept
    {
        return *PolicyT::entry(node_);
    }
    Pointer
    operator ->() const noexcept
    {
        return PolicyT::entry(node_);
    }
    ConstForwardIterator&
    operator ++() noexcept
    {
        node_ = PolicyT::next(node_);
        return *this;
    }
    ConstForwardIterator
    operator ++(int) noexcept
    {
        ConstForwardIterator tmp{*this};
        node_ = PolicyT::next(node_);
        return tmp;
    }
    bool
    operator ==(ConstForwardIterator rhs) const noexcept
    {
        return node_ == rhs.node_;
    }
    bool
    operator !=(ConstForwardIterator rhs) const noexcept
    {
        return node_ != rhs.node_;
    }
};

template <typename PolicyT>
inline bool
operator ==(ForwardIterator<PolicyT> lhs, ConstForwardIterator<PolicyT> rhs) noexcept
{
    return ConstForwardIterator<PolicyT>{lhs} == rhs;
}

template <typename PolicyT>
inline bool
operator ==(ConstForwardIterator<PolicyT> lhs, ForwardIterator<PolicyT> rhs) noexcept
{
    return lhs == ConstForwardIterator<PolicyT>{rhs};
}

template <typename PolicyT>
inline bool
operator !=(ForwardIterator<PolicyT> lhs, ConstForwardIterator<PolicyT> rhs) noexcept
{
    return ConstForwardIterator<PolicyT>{lhs} != rhs;
}

template <typename PolicyT>
inline bool
operator !=(ConstForwardIterator<PolicyT> lhs, ForwardIterator<PolicyT> rhs) noexcept
{
    return lhs != ConstForwardIterator<PolicyT>{rhs};
}

/** @} */

/**
 * @addtogroup String
 * @{
 */

template <typename T>
class JoinIterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
    std::ostream* os_;
    const char* delim_;
    bool first_;
public:
    JoinIterator(std::ostream& os, const char* delim)
        : os_(&os),
          delim_(delim),
          first_(true)
    {
    }
    JoinIterator&
    operator =(T value)
    {
        if (first_)
            first_ = false;
        else
            *os_ << delim_;
        *os_ << value;
        return *this;
    }
    JoinIterator&
    operator *()
    {
        return *this;
    }
    JoinIterator&
    operator ++()
    {
        return *this;
    }
    JoinIterator&
    operator ++(int)
    {
        return *this;
    }
};

/** @} */

} // dbr

#endif // DBRPP_ASH_ITER_HPP
