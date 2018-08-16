#pragma once
#include <choose.h>

#include <list>
#include <map>
#include <memory>
#include <cassert>

namespace preftree {

using Digit = uint8_t;
class Path: public std::list<Digit> {
public:
    Path(uint64_t val) {
        for(; val; val /= 10)
            push_front(val%10);
    }
};

using Key = uint64_t;

template<typename VAL>
class Node {
public:
    using Children = std::map<Digit, std::shared_ptr<Node>>;

    Node(VAL&& val): val_{std::move(val)} {}

    auto parent() const -> const Node* { return parent_; }
    auto parent() -> Node* { return parent_; }

    auto valref() const -> const VAL& { return val_; }
    auto valref() -> VAL& { return val_; }

    auto valptr() const -> const VAL* { return &val_; }
    auto valptr() -> VAL* { return &val_; }

    auto children() const -> const Children& { return children_; }
    auto children() -> Children& { return children_; }

private:
    std::weak_ptr<Node>     parent_{};
    Children                children_;
    VAL                     val_;
};

template <class T, bool isconst = false> 
class preftree_iterator {
public:
    using value_type = T;
    using reference = typename choose<isconst, const T&, T&>::type;
    using pointer = typename choose<isconst, const T*, T*>::type;
    using wptr = std::weak_ptr<typename choose<isconst, const T, T>::type>;

    preftree_iterator(std::weak_ptr<Node<T>> n): node_{n} {}
    preftree_iterator(preftree_iterator<T, false> it): node_{it.operator->()} {}
    auto operator->() const -> pointer { return node_->valptr(); }
    auto operator*() const -> reference { return node_->valref(); }
    preftree_iterator& operator--() { node_ = node_->parent(); }
    bool operator==(const preftree_iterator& other) const { return node_ == other.node_; }
    bool operator!=(const preftree_iterator& other) const { return !(*this == other); }
    auto node() const -> std::weak_ptr<const Node<T>>& { return node_; }
private:
    std::weak_ptr<Node<T>> node_{};
};

template<typename VAL>
class Preftree {
public:
    using const_iterator = preftree_iterator<VAL, true>;
    using iterator = preftree_iterator<VAL, false>;

    Preftree() = default;
    auto emplace(Key, VAL&&) -> std::pair<bool, iterator>;

    auto find(Key) -> iterator;
    auto find(Key) const -> const_iterator;

    auto find_closest(const Path&, Path::const_iterator&) const -> const_iterator;
    auto find_closest(const Path&, Path::const_iterator&, const_iterator start) const -> const_iterator;

    auto begin() -> iterator { return iterator(root_); }
    auto cbegin() -> const_iterator { return const_iterator(root_); }

    auto end() -> iterator { return iterator(nullptr); }
    auto cend() -> const_iterator { return const_iterator(nullptr); }

private:
    std::shared_ptr<Node<VAL>> root_;
};


template<typename VAL>
auto Preftree<VAL>::find_closest(const Path& p, Path::const_iterator& pit) const
    -> const_iterator
{
    return this->find_closest(p, pit, cbegin());
}

template<typename VAL>
auto Preftree<VAL>::find_closest(const Path& p, Path::const_iterator& pit, const_iterator tit) const
    -> const_iterator
{
    if (pit == p.end())
        return tit;

    assert(tit != cend());

    auto d = Digit(*pit);
    const auto& children = tit.node().children();
    auto child = children.find(d);
    if (child == children.end())
        return tit;
    return find_closest(p, ++pit, {child->second});
}

template<typename VAL>
auto Preftree<VAL>::find(Key k) const
    -> const_iterator
{
    Path p(k);
    auto pi = p.begin();
    auto ti = find_closest(p, pi, cbegin());
    if (pi == p.end())
        return ti;
    return cend();
}

}
