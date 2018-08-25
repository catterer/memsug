#pragma once
#include <preftree.hh>
#include <optional.hh>
#include <save.hh>
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

struct Word: public save::serializable {
    Word(const save::blob& root) { load(root); }
    Word(WordId id, Key key, const std::string& str):
        id{id}, key{key}, str{str} {}

    auto dump() const -> save::blob override;
    void load(const save::blob&) override;

    friend std::ostream& operator<<(std::ostream&, const Word&);

    WordId      id;
    Key         key;
    std::string str;
};

class DictEntry: public save::serializable{
public:
    DictEntry(const Word& w): word_{w} {}
    auto dump() const -> save::blob override { return word_.dump(); }
    void load(const save::blob& root) override { return word_.load(root); }
    auto word() const -> const Word& { return word_; }

private:
    Word    word_;
};

class Dict:
    public std::unordered_map<WordId, std::shared_ptr<DictEntry>>,
    public save::serializable
{
public:
    using Idxstr = std::unordered_map<std::string, std::shared_ptr<DictEntry>>;
    Dict(const Alphabet& ab): alphabet_{ab} {}

    auto dump() const -> save::blob override;
    void load(const save::blob&) override;

    void update(const std::string& textfile);
    void insert(Word&&);
    void consider_sentence(const std::string&);
    void consider_word(const std::string&);
    auto idxstr() const -> const Idxstr& { return idxstr_; }

private:
    static const size_t max_file_size_ = 100*1024*1024;

    Alphabet alphabet_;
    WordId last_id_{0};
    Idxstr idxstr_;
};

}
