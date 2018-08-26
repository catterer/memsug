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

auto Suger::suggest_word(const text::Number& num)
    -> optional<std::string>
{
    auto sms = tree_.find(num);
    if (sms == tree_.end())
        return {};
    assert(!sms->value().empty());
    return {dict_[sms->value().begin()->second]->word().str};
}

}
