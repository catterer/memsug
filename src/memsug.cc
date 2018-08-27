#include <memsug.hh>

namespace memsug {

Suger::Suger(text::Dict&& d):
    dict_{std::move(d)}
{
    for (const auto& p: dict_) {
        const auto& word = p.second->word();
        auto tp = tree_.emplace(word.num, Synonyms{{1, word.id}});
        if (!tp.second)
            tp.first->value().emplace(1, word.id);
    }
}

auto Suger::suggest_word(const text::Number& num) const
    -> optional<std::string>
{
    auto sms = tree_.find(num);
    if (sms == tree_.end())
        return {};
    assert(!sms->value().empty());
    return {dict_[sms->value().begin()->second]->word().str};
}

auto Suger::maximize_word_length(const text::Number& num) const
    -> optional<std::vector<std::string>>
{
    std::vector<std::string> res;
    auto pi = num.begin();
    for (auto ti = tree_.find_closest(num, pi);
            pi != num.end and ti != tree_.end();
            ti = tree_.find_closest(num, pi))
    {
        Variants vs{};
        for (const auto& p: ti->value())
            vs.emplace_back(dict_[p.second]->word().str());
        res.emplace_back(std::move(vs));
    }

    return res;
}

}
