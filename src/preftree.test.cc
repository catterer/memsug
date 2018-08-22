#include <preftree.hh>
#include <string>
#include <gtest/gtest.h>

using namespace preftree;

TEST(Preftree, basic_one) {
    Preftree<std::string> pt{};
    pt.emplace(1, "1");
    ASSERT_EQ(pt.find(1)->valref(), "1");
}

TEST(Preftree, zero) {
    Preftree<std::string> pt{};
    pt.emplace(0, "0");
    ASSERT_EQ(pt.find(0)->valref(), "0");
}

TEST(Preftree, basic_mult) {
    Preftree<std::string> pt{};
    const int n = 30;
    for (int i = 0; i < n; i++)
        pt.emplace(i, std::to_string(i));

    for (int i = 0; i < n; i++)
        ASSERT_EQ(pt.find(i)->valref(), std::to_string(i));
}

TEST(Preftree, basic_deep) {
    Preftree<std::string> pt{};
    pt.emplace(1234567890, "1234567890");
    ASSERT_EQ(pt.find(1234567890)->valref(), "1234567890");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
