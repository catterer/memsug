#pragma once
#include <choose.h>

#include <list>
#include <map>
#include <memory>
#include <cassert>
#include <iostream>

namespace preftree {

using Digit = uint8_t;
class Path: public std::list<Digit> {
public:
    Path(uint64_t val):
        key_{val}
    {
        if (!val)
            push_front(0);

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

    void val(VAL&& v) { val_ = std::move(v); }

    auto children() const -> const Children& { return children_; }
    auto children() -> Children& { return children_; }

    void print(std::ostream&, unsigned) const;

private:
    std::weak_ptr<Node<VAL>>    parent_{};
    Children                children_;
    VAL                     val_;
};

template<typename VAL>
class Preftree {
public:
    using iterator = std::shared_ptr<Node<VAL>>;
    using const_iterator = const std::shared_ptr<Node<VAL>>;

    Preftree() = default;
    auto emplace(Key, VAL&&) -> std::pair<iterator, bool>;
    auto grow(iterator, const Path&, Path::const_iterator, VAL&&) -> iterator;

    auto find(Key) -> iterator;
    auto find(Key) const -> const_iterator;

    auto find_closest(const Path&, Path::const_iterator&) -> iterator;
    auto find_closest(const Path&, Path::const_iterator&) const -> const_iterator;
    auto find_closest(const Path&, Path::const_iterator&, const_iterator start) const -> const_iterator;

    auto begin() -> iterator { return root_; }
    auto cbegin() const -> const_iterator { return root_; }

    auto end() -> iterator { return {}; }
    auto cend() const -> const_iterator { return {}; }

    void print(std::ostream& out) const { root_->print(out, 0); }

private:
    std::shared_ptr<Node<VAL>> root_{std::make_shared<Node<VAL>>(Node<VAL>({}, {}))};
};

template<typename VAL>
auto Preftree<VAL>::emplace(Key k, VAL&& v)
    -> std::pair<iterator, bool>
{
    auto path = Path(k);
    auto pit = path.cbegin();
    auto ti = find_closest(path, pit);
    if (pit == path.cend())
        return std::make_pair(ti, false);
    return std::make_pair(grow(ti, path, pit, std::move(v)), true);
}

template<typename VAL>
auto Preftree<VAL>::grow(iterator tit, const Path& path, Path::const_iterator pit, VAL&& v)
    -> iterator
{
    if (pit == path.cend()) {
        tit->val(std::move(v));
        return tit;
    }

    auto& children = tit->children();
    auto pr = children.emplace(*pit, std::make_shared<Node<VAL>>(tit, VAL()));
    assert(pr.second); // bug in find_closest
    return grow(pr.first->second, path, ++pit, std::move(v));
}

template<typename VAL>
auto Preftree<VAL>::find_closest(const Path& p, Path::const_iterator& pit)
    -> iterator
{
    auto ci = const_cast<const Preftree<VAL>&>(*this).find_closest(p, pit);
    return iterator(ci, const_cast<Node<VAL>*>(ci.get()));
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
    const auto& children = tit->children();
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

template<typename VAL>
auto Preftree<VAL>::find(Key k)
    -> iterator
{
    Path p(k);
    auto pi = p.cbegin();
    auto ti = find_closest(p, pi);
    if (pi == p.end())
        return ti;
    return cend();
}

template<typename VAL>
void Node<VAL>::print(std::ostream& out, unsigned bkts_cnt) const {
    static const char bkts[] = "()[]{}";
    out << bkts[bkts_cnt] << val_ << ": ";
    for (const auto& c: children()) {
        out << int(c.first) << "=";
        c.second->print(out, (bkts_cnt+2) % (sizeof(bkts)-1));
        out << " ";
    }
    out << bkts[bkts_cnt+1];
}

}
