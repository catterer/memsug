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
    Path(uint64_t val):
        key_{val}
    {
        for(; val; val /= 10)
            push_front(val%10);
    }

    auto key() const -> uint64_t { return key_; }
private:
    uint64_t key_;
};

using Key = uint64_t;

template<typename VAL>
class Node {
public:
    using Children = std::map<Digit, std::shared_ptr<Node<VAL>>>;

    Node(std::weak_ptr<Node<VAL>> parent, VAL&& val):
        parent_{parent},
        val_{std::move(val)}
    { }

    auto parent() const -> const Node* { return parent_; }
    auto parent() -> Node* { return parent_; }

    auto valref() const -> const VAL& { return val_; }
    auto valref() -> VAL& { return val_; }

    auto valptr() const -> const VAL* { return &val_; }
    auto valptr() -> VAL* { return &val_; }

    auto children() const -> const Children& { return children_; }
    auto children() -> Children& { return children_; }

private:
    std::weak_ptr<Node<VAL>>    parent_{};
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
    auto node() const -> std::weak_ptr<Node<T>>& { return node_; }
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
    auto emplace(iterator tit, const Path& path, Path::const_iterator pit, VAL&& v) -> std::pair<bool, iterator>;

    auto find(Key) -> iterator;
    auto find(Key) const -> const_iterator;

    auto find_closest(const Path&, Path::const_iterator&) -> iterator;
    auto find_closest(const Path&, Path::const_iterator&) const -> const_iterator;
    auto find_closest(const Path&, Path::const_iterator&, const_iterator start) const -> const_iterator;

    auto begin() -> iterator { return iterator(root_); }
    auto cbegin() -> const_iterator { return const_iterator(root_); }

    auto end() -> iterator { return iterator(nullptr); }
    auto cend() const -> const_iterator { return const_iterator(nullptr); }

private:
    std::shared_ptr<Node<VAL>> root_{std::make_shared<Node<VAL>>(Node<VAL>({}, {}))};
};


template<typename VAL>
auto Preftree<VAL>::emplace(Key k, VAL&& v)
    -> std::pair<bool, iterator>
{
    auto path = Path(k);
    auto pit = path.cbegin();
    auto ti = find_closest(path, pit);
    return emplace(ti, path, pit, std::move(v));
}

template<typename VAL>
auto Preftree<VAL>::emplace(iterator tit, const Path& path, Path::const_iterator pit, VAL&& v)
    -> std::pair<bool, iterator>
{
    if (pit == path.cend())
        return std::make_pair(false, tit);

    auto& children = tit.node().children();
    auto pr = children.emplace(*pit, std::make_shared<Node<VAL>>(tit.node(), std::move(v)));
    assert(pr.first); // bug in find_closest
    return std::make_pair(true, emplace(iterator(pr.second), path, ++pit, std::move(v)).second);
}

template<typename VAL>
auto Preftree<VAL>::find_closest(const Path& p, Path::const_iterator& pit)
    -> iterator
{
    auto const_tit = find_closest(p, pit, cbegin());
    return iterator(const_tit.node());
}

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
