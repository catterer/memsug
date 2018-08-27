#pragma once
#include <memsug.hh>
#include <numeric>

namespace memsug {

class Donsug: public ::testing::Test {
public:
    Donsug() {
        text::Dict d{text::Alphabet::classic_ru()};
        d.update(ROOT "/misc/don_short.txt");
        suger_ = Suger::create(std::move(d));
    }

    std::shared_ptr<Suger> suger_;
};

TEST_F(Donsug, simple) {
    ASSERT_EQ(*suger_->suggest_word("9167"), "батюшка");
}

TEST_F(Donsug, mwl) {
    auto res = suger_->maximize_word_length("12");
    ASSERT_EQ(res->size(), 1U);
    const auto vs = *res->begin();
    ASSERT_EQ(vs.size(), 4U);
    auto sorted = std::accumulate(vs.begin(), vs.end(), std::set<std::string>(),
            [] (auto&& accum, const auto& word) { accum.emplace(word); return std::move(accum); });
    ASSERT_EQ(sorted, (std::set<std::string>{"Дона", "Дону", "Дон", "дна"}));
}

}
