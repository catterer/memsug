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

TEST(AdjMatrix, serialization) {
    auto m = save::dumpload(AdjMatrix{{6, {5,6,7}}, {5, {1,2,3}}});
    std::string expected =
        "{\n"
        "    \"6\": {\n"
        "        \"5\": \"\",\n"
        "        \"6\": \"\",\n"
        "        \"7\": \"\"\n"
        "    },\n"
        "    \"5\": {\n"
        "        \"1\": \"\",\n"
        "        \"2\": \"\",\n"
        "        \"3\": \"\"\n"
        "    }\n"
        "}\n";


    std::stringstream sm;
    save::pt::write_json(sm, m.dump());

    ASSERT_EQ(sm.str(), expected);
}

class DictDonShort: public ::testing::Test {
public:
    DictDonShort():
        dict(Alphabet::classic_ru())
    {
        dict.update(text_path);
    }

    Dict dict;
    const char* text_path = ROOT "/misc/don_short.txt";
};

TEST_F(DictDonShort, adjmx) {
    auto is_adj = [&] (const std::string& a_str, const std::string& b_str) -> bool {
        WordId a, b;

        if (!dict.idxstr().count(a_str)) return false;
        else a = dict.idxstr().at(a_str)->word().id;

        if (!dict.idxstr().count(b_str)) return false;
        else b = dict.idxstr().at(b_str)->word().id;

        if (!dict.adjmx().count(a))
            return false;
        return dict.adjmx().at(a).count(b);
    };

    ASSERT_TRUE(is_adj("славная", "землюшка"));
    ASSERT_TRUE(is_adj("землюшка", "лошадиными"));
    ASSERT_TRUE(is_adj("землюшка", "казацкими"));
    ASSERT_TRUE(is_adj("Украшен", "то"));

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

TEST_F(DictDonShort, update_after_dumpload) {
    auto check = [&] (const std::string& w) {
        auto d = save::dumpload(dict);
        auto ei = d.idxstr().find(w);
        ASSERT_NE(ei, d.idxstr().end());
        ASSERT_EQ(ei->second->word().str, w);
    };
    check("батюшка");
    check("Наполнена");
    check("копытами");
    check("Посередь");
}

TEST_F(DictDonShort, update_idempotency) {
    std::stringstream out_a;
    save::pt::write_json(out_a, dict.dump());

    dict.update(text_path);

    std::stringstream out_b;
    save::pt::write_json(out_b, dict.dump());

    ASSERT_EQ(out_a.str(), out_b.str());
}

}
