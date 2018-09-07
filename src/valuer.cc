#include <valuer.hh>
#include <stdexcept>
#include <limits>
#include <queue>

namespace valuer {

static const Vtx BEGIN = 0;
static const Vtx END = 1;

static const Weight INF = std::numeric_limits<Weight>::max();
static const Weight DEF = 3;

void Matrix::connect(Vtx from, Vtx to, Weight weight) {
    (*this)[from].emplace(Edge{to, weight});
}

Valuer::Valuer(const AdjMatrix& adjmx):
    adjmx_{adjmx}
{ }

auto Valuer::weight(Vtx from, Vtx to) const
    -> Weight
{
    auto from_wid = vxwid_.at(from);
    auto to_wid = vxwid_.at(to);
    auto ai = adjmx_.find(from_wid);
    if (ai != adjmx_.end() and ai->second.count(to_wid))
        return 1;
    return DEF;
}

void Valuer::update(const std::vector<Synonyms>& sms) {
    if (sms.empty())
        return;

    std::vector<std::vector<Vtx>> vxm;

    size_t vxwid_new_size = vxwid_.size() + RESERVED;

    for (const auto& sm: sms)
        vxwid_new_size += sm.size();

    vxwid_.resize(vxwid_new_size, 0);

    for (const auto& sm: sms) {
        if (sm.empty())
            throw std::invalid_argument("empty row");

        std::vector<Vtx> row;
        for (const auto& w: sm) {
            row.emplace_back(last_vtx_id_);
            vxwid_[last_vtx_id_++] = w;
        }
        vxm.emplace_back(std::move(row));
    }

    matrix_.resize(vxwid_new_size, {});

    for (auto vx: *vxm.begin())
        matrix_.connect(BEGIN, vx, DEF);

    for (auto row_it = vxm.begin(); row_it != vxm.end(); row_it++) {
        auto next_row = std::next(row_it);
        if (next_row == vxm.end())
            break;
        for (auto from: *row_it)
            for (auto to: *next_row)
                matrix_.connect(from, to, weight(from, to));
    }

    for (auto vx: *vxm.rbegin())
        matrix_.connect(vx, END, DEF);

    matrix_[END] = {};
}

std::ostream& operator<<(std::ostream& out, const std::unordered_map<Vtx, Edge>& routes) {
    for (const auto& p: routes)
        out << p.first << "->" << p.second.vtx << ":" << p.second.weight << " ";
    return out;
}

auto Valuer::solve() const
    -> Solution
{
    std::vector<Edge> routes{};
    routes.resize(matrix_.size(), Edge{0, INF});
    routes[0].weight = 0;
    std::queue<Vtx> todo{};
    todo.push(0);
    std::vector<bool> passed(matrix_.size(), false);

    for (; not todo.empty(); todo.pop()) {
        auto current = todo.front();
        const auto& neighb = matrix_.at(current);
        for (auto n: neighb) {
            auto& edge = routes[n.vtx];
            auto new_weight = routes.at(current).weight + n.weight;
            if (new_weight < edge.weight)
                edge = Edge{current, new_weight};
            if (not passed[current])
                todo.push(n.vtx);
        }
        passed[current] = true;
    }

    Solution sol{};
    for (auto vtx = routes[END].vtx; vtx != BEGIN; vtx = routes[vtx].vtx)
        sol.push_front(vxwid_.at(vtx));

    return sol;
}

}
