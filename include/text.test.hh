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

TEST(Dict, update) {
    Dict d(Alphabet::classic_ru());
    d.update(ROOT "/misc/don_short.txt");
    auto check = [&d] (const std::string& w) {
        auto ei = d.idxstr().find(w);
        ASSERT_NE(ei, d.idxstr().end());
        ASSERT_EQ(ei->second->word().str, w);
    };
    check("батюшка");
    check("Наполнена");
    check("копытами");
    check("Посередь");
}

}
