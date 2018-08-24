#include <text.hh>
#include <gtest/gtest.h>

namespace text {

TEST(Alphabet, map_simple) {
    ASSERT_EQ(*Alphabet::classic_ru().map("автоэлектростеклоподъемники"), 815719017591327LU);
}

}
