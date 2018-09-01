#include <text.hh>
#include <gtest/gtest.h>

namespace text {

TEST(Alphabet, map_ru) {
    ASSERT_EQ(Alphabet::classic_ru().map("автоэлектростеклоподъемники"), "815719017591327");
}

TEST(Alphabet, map_en) {
    ASSERT_EQ(Alphabet::classic_en().map("anonimity"), "1909");
}


TEST(Alphabet, map_ru_case) {
    ASSERT_EQ(Alphabet::classic_ru().map("АВТОЭЛЕКТРОСТЕКЛОПОДЪЕМНИКИ"), "815719017591327");
}

TEST(Alphabet, map_en_case) {
    ASSERT_EQ(Alphabet::classic_en().map("ANONIMITY"), "1909");
}

TEST(Alphabet, dl_map_ru) {
    ASSERT_EQ(save::dumpload(Alphabet::classic_ru()).map("автоэлектростеклоподъемники"), "815719017591327");
}

TEST(Alphabet, dl_map_en) {
    ASSERT_EQ(save::dumpload(Alphabet::classic_en()).map("anonimity"), "1909");
}


TEST(Alphabet, dl_map_ru_case) {
    ASSERT_EQ(save::dumpload(Alphabet::classic_ru()).map("АВТОЭЛЕКТРОСТЕКЛОПОДЪЕМНИКИ"), "815719017591327");
}

TEST(Alphabet, dl_map_en_case) {
    ASSERT_EQ(save::dumpload(Alphabet::classic_en()).map("ANONIMITY"), "1909");
}

class DictDonShort: public ::testing::Test {
public:
    DictDonShort():
        dict(Alphabet::classic_ru())
    {
        dict.update(ROOT "/misc/don_short.txt", adjmx);
    }

    AdjMatrix adjmx;
    Dict dict;
};

TEST_F(DictDonShort, adjmx) {
    auto is_adj = [&] (const std::string& a_str, const std::string& b_str) -> bool {
        WordId a, b;

        if (!dict.idxstr().count(a_str)) return false;
        else a = dict.idxstr().at(a_str)->word().id;

        if (!dict.idxstr().count(b_str)) return false;
        else b = dict.idxstr().at(b_str)->word().id;

        if (!adjmx.count(a))
            return false;
        return adjmx[a].count(b);
    };

    ASSERT_TRUE(is_adj("славная", "землюшка"));
    ASSERT_TRUE(is_adj("землюшка", "лошадиными"));
    ASSERT_TRUE(is_adj("землюшка", "казацкими"));
    ASSERT_TRUE(is_adj("Дона", "бела"));

    ASSERT_FALSE(is_adj("землюшка", "молодыми"));
    ASSERT_FALSE(is_adj("Дон", "Ой"));
    ASSERT_FALSE(is_adj("Дону", "тихому"));
}

TEST_F(DictDonShort, update) {
    auto check = [&] (const std::string& w) {
        auto ei = dict.idxstr().find(w);
        ASSERT_NE(ei, dict.idxstr().end());
        ASSERT_EQ(ei->second->word().str, w);
    };
    check("батюшка");
    check("Наполнена");
    check("копытами");
    check("Посередь");
}

}
