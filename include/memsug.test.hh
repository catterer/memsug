#pragma once
#include <memsug.hh>

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

}
