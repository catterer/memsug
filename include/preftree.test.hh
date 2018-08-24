#include <preftree.hh>
#include <string>
#include <gtest/gtest.h>

namespace preftree {

TEST(Preftree, basic_one) {
    Preftree<std::string> pt{};
    pt.emplace(1, "1");
    ASSERT_EQ(*pt.find(1)->value(), "1");
}

TEST(Preftree, zero) {
    Preftree<std::string> pt{};
    pt.emplace(0, "0");
    ASSERT_EQ(*pt.find(0)->value(), "0");
}

TEST(Preftree, basic_mult) {
    Preftree<std::string> pt{};
    const int n = 30;
    for (int i = 0; i < n; i++)
        pt.emplace(i, std::to_string(i));

    for (int i = 0; i < n; i++)
        ASSERT_EQ(*pt.find(i)->value(), std::to_string(i));
}

TEST(Preftree, basic_deep) {
    Preftree<std::string> pt{};
    pt.emplace(1234567890, "1234567890");
    ASSERT_EQ(*pt.find(1234567890)->value(), "1234567890");
}

TEST(Preftree, randomized) {
    Preftree<std::string> pt{};
    std::set<std::string> alphasorted;
    for (int i = 0; i < 100; i++) {
        int val = rand() % 10000;
        pt.emplace(val, std::to_string(val));
        alphasorted.emplace(std::to_string(val));
    }

    std::list<std::string> l;
    pt.pass_depth([&](const auto& node, unsigned lvl) {
        if (not node.empty())
            l.emplace_back(*node.value());
    });

    auto lit = l.begin();
    for (const auto& std: alphasorted) {
        ASSERT_NE(lit, l.end());
        ASSERT_EQ(*lit, std);
        ++lit;
    }
    ASSERT_EQ(lit, l.end());
}

}
