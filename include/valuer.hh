#pragma once
#include <memsug.hh>
#include <list>
#include <vector>
#include <unordered_map>

namespace valuer {

using memsug::Synonyms;
using text::WordId;
using text::AdjMatrix;
using Solution = std::list<WordId>;
using Weight = unsigned;
using Vtx = unsigned;

struct Edge {
    Vtx vtx;
    Weight weight;

    bool operator<(const Edge& other) const { return vtx < other.vtx; }
};

class Matrix: public std::unordered_map<Vtx, std::set<Edge>> {
public:
    Matrix() = default;
    void connect(Vtx from, Vtx to, Weight);
};

class Valuer {
public:
    Valuer(const AdjMatrix&);
    void update(const std::vector<Synonyms>&);
    auto solve() const -> Solution;

private:
    auto weight(Vtx from, Vtx to) const -> Weight;

    Matrix matrix_;
    std::unordered_map<Vtx, WordId> vxwid_;
    Vtx last_vtx_id_{1};
    const AdjMatrix& adjmx_;
};

}
