#include <text.hh>
#include <gtest/gtest.h>

namespace text {

TEST(Alphabet, map_ru) {
    ASSERT_EQ(*Alphabet::classic_ru().map("автоэлектростеклоподъемники"), 815719017591327LU);
}

TEST(Alphabet, map_en) {
    ASSERT_EQ(*Alphabet::classic_en().map("anonimity"), 1909LU);
}


TEST(Alphabet, map_ru_case) {
    ASSERT_EQ(*Alphabet::classic_ru().map("АВТОЭЛЕКТРОСТЕКЛОПОДЪЕМНИКИ"), 815719017591327LU);
}

TEST(Alphabet, map_en_case) {
    ASSERT_EQ(*Alphabet::classic_en().map("ANONIMITY"), 1909LU);
}
}
