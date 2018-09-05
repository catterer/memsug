#include <memsug.hh>

namespace memsug {

Suger::Suger(text::Dict&& d):
    dict_{std::move(d)}
{
    for (const auto& p: dict_) {
        const auto& word = p.second->word();
        auto tp = tree_.emplace(word.num, Synonyms{word.id});
        if (!tp.second)
            tp.first->value().emplace_back(word.id);
    }
}

auto Suger::suggest_word(const text::Number& num) const
    -> optional<std::string>
{
    auto sms = tree_.find(num);
    if (sms == tree_.end())
        return {};
    assert(!sms->value().empty());
    return {dict_.at(*sms->value().begin())->word().str};
}

auto Suger::maximize_word_length(const text::Number& num) const
    -> optional<std::vector<Synonyms>>
{
    std::vector<Synonyms> res;
    auto pi = num.begin();
    do {
        auto ti = tree_.find_closest(num, pi);
        if (ti == tree_.end())
            return {};

        res.emplace_back(ti->value());
    } while (pi != num.end());

    return res;
}

}
