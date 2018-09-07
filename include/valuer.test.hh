#include <valuer.hh>
#include <gtest/gtest.h>

namespace valuer {

TEST(Valuer, simple_route) {
    AdjMatrix adjmx;
    adjmx[1].emplace(3);

    Valuer valr{adjmx};
    valr.update({{1}, {2,3}, {4}});

    auto sol = valr.solve();
    ASSERT_EQ(sol, (std::list<WordId>{1, 3, 4}));
}

TEST(Valuer, simple_route_shuffle_numbers) {
    AdjMatrix adjmx;
    adjmx[101].emplace(103);

    Valuer valr{adjmx};
    valr.update({{101}, {102,103}, {104}});

    auto sol = valr.solve();
    ASSERT_EQ(sol, (std::list<WordId>{101, 103, 104}));
}

TEST(Valuer, two_updates) {
    AdjMatrix adjmx;
    adjmx[101].emplace(103);
    adjmx[105].emplace(106);
    adjmx[106].emplace(108);

    Valuer valr{adjmx};
    valr.update({{101}, {102,103}, {104}});

    auto sol = valr.solve();
    ASSERT_EQ(sol, (std::list<WordId>{101, 103, 104}));

    valr.update({{105}, {106,107}, {108}});

    sol = valr.solve();
    ASSERT_EQ(sol, (std::list<WordId>{105, 106, 108}));
}

}

