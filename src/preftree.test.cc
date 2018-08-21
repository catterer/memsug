#include <preftree.hh>
#include <string>
#include <gtest/gtest.h>

using namespace preftree;

TEST(Preftree, basic) {
    Preftree<std::string> pt{};
    pt.emplace(1, "1");
    ASSERT_EQ(*pt.find(1), "1");
}

int main(int argc, char** argv) {
    return RUN_ALL_TESTS();
}
