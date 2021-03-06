#pragma once
#include <text.hh>
#include <preftree.hh>
#include <optional.hh>

#include <stdint.h>
#include <map>
#include <vector>
#include <list>

namespace memsug {

using Weight = uint32_t;
using ::text::WordId;

using Synonyms = std::vector<WordId>;

class Suger: public std::enable_shared_from_this<Suger> {
public:
    template<typename ...Args>
    static auto create(Args&& ...args) -> std::shared_ptr<Suger> {
        return std::shared_ptr<Suger>(new Suger(std::forward<Args>(args)...));
    }

    auto suggest_word(const text::Number&) const -> optional<std::string>;
    auto maximize_word_length(const text::Number&, int shorten_first_word = 0) const -> optional<std::vector<Synonyms>>;
    auto dict() const -> const text::Dict& { return dict_; }

private:
    Suger(text::Dict&&);

    text::Dict dict_;
    preftree::Preftree<Synonyms> tree_;
};

}
