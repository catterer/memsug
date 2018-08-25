#pragma once
#include <preftree.hh>
#include <optional.hh>
#include <unordered_map>

namespace text {

using preftree::Key;

class Alphabet:
    public std::unordered_map<std::string, preftree::Digit>
{
public:
    static auto classic_ru() -> Alphabet;
    static auto classic_en() -> Alphabet;

    Alphabet(const char* locale, std::vector<std::string> descriptor);
    auto map(const std::string& word) -> optional<Key>;

private:
    const char* locale_{};
};

using WordId = uint32_t;

struct Word {
    WordId      id;
    Key         key;
    std::string str;
};

class DictEntry {
public:
    DictEntry(const Word& w): word_{w} {}
    auto word() const -> const Word& { return word_; }

private:
    Word    word_;
};

class Dict:
    public std::unordered_map<WordId, std::shared_ptr<DictEntry>>
{
public:
    Dict(const Alphabet& ab): alphabet_{ab} {}
    void update(const std::string& textfile);
    void insert(Word&&);
    void consider_sentence(const std::string&);
    void consider_word(const std::string&);

private:
    static const size_t max_file_size_ = 100*1024*1024;

    Alphabet alphabet_;
    WordId last_id_{0};
    std::unordered_map<std::string, std::shared_ptr<DictEntry>>     idxstr_;
};

}
