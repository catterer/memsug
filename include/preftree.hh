#pragma once
#include <list>
#include <map>
#include <memory>

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

template<typename VAL>
class Preftree {
public:
    class iterator {
    public:
        iterator(std::weak_ptr<Node<VAL>> n): node_{n} {}
        auto operator->() -> VAL* { return node_->valptr(); }
        auto operator*() -> VAL& { return node_->valref(); }
        iterator& operator--() { node_ = node_->parent(); }
        bool operator==(const iterator& other) const { return node_ == other.node_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
        auto node() const -> const std::weak_ptr<Node<VAL>>& { return *node_; }
        auto node() -> std::weak_ptr<Node<VAL>>& { return *node_; }
    private:
        std::weak_ptr<Node<VAL>> node_{};
    };

    class const_iterator {
    public:
        const_iterator(std::weak_ptr<const Node<VAL>> n): node_{n} {}
        const_iterator(iterator&& it): node_{it.operator->()} {}
        auto operator->() const -> const VAL* { return node_->valptr(); }
        auto operator*() const -> const VAL& { return node_->valref(); }
        const_iterator& operator--() { node_ = node_->parent(); }
        bool operator==(const const_iterator& other) const { return node_ == other.node_; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
        auto node() const -> std::weak_ptr<const Node<VAL>>& { return *node_; }
    private:
        std::weak_ptr<const Node<VAL>> node_{};
    };

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

}
