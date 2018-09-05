#pragma once
#include <memsug.hh>
#include <numeric>

namespace memsug {

class Donsug: public ::testing::Test {
public:
    Donsug() {
        text::AdjMatrix m;
        text::Dict d{text::Alphabet::classic_ru()};
        d.update(ROOT "/misc/don_short.txt", m);
        suger_ = Suger::create(std::move(d));
    }

    std::shared_ptr<Suger> suger_;
};

TEST_F(Donsug, simple) {
    ASSERT_EQ(*suger_->suggest_word("9167"), "батюшка");
}

TEST_F(Donsug, mwl_variants) {
    auto res = suger_->maximize_word_length("12");
    ASSERT_EQ(res->size(), 1U);
    const auto sms = *res->begin();
    ASSERT_EQ(sms.size(), 4U);
    auto sorted = std::accumulate(sms.begin(), sms.end(), std::set<std::string>(),
            [&] (auto&& accum, auto wid) {
                accum.emplace(suger_->dict().at(wid)->word().str);
                return std::move(accum);
            });
    ASSERT_EQ(sorted, (std::set<std::string>{"Дона", "Дону", "Дон", "дна"}));
}

TEST_F(Donsug, mwl_length) {
    auto res = suger_->maximize_word_length("03567002");
    ASSERT_EQ(res->size(), 2U);
    auto vsi = res->begin();
    ASSERT_EQ(vsi->size(), 1U);
    ASSERT_EQ(suger_->dict().at(*vsi->begin())->word().str, "землюшка");
    ++vsi;
    ASSERT_EQ(vsi->size(), 1U);
    ASSERT_EQ(suger_->dict().at(*vsi->begin())->word().str, "засеяна");
}

}
