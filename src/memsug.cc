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
    return {dict_.at(sms->value().begin()->second)->word().str};
}

auto Suger::maximize_word_length(const text::Number& num) const
    -> optional<std::vector<Variants>>
{
    std::vector<Variants> res;
    auto pi = num.begin();
    do {
        auto ti = tree_.find_closest(num, pi);
        if (ti == tree_.end())
            return {};

        Variants vs{};
        for (const auto& p: ti->value())
            vs.emplace_back(dict_.at(p.second)->word().str);
        res.emplace_back(std::move(vs));
    } while (pi != num.end());

    return res;
}

}
