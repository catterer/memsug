#pragma once
#include <save.hh>
#include <unordered_map>
#include <set>
#include <vector>

namespace text {

using Number = std::string;

class Alphabet:
    public std::unordered_map<std::string, uint8_t>,
    public save::serializable
{
public:
    static auto classic_ru() -> Alphabet;
    static auto classic_en() -> Alphabet;
    static auto by_name(const std::string&) -> Alphabet;

    Alphabet(const save::blob& b) { load(b); }
    Alphabet(std::vector<std::string>&& descriptor);
    auto map(const std::string& word) const -> Number;

    auto dump() const -> save::blob override;
    void load(const save::blob&) override;
};

using WordId = uint32_t;

struct Word: public save::serializable {
    Word(const save::blob& root) { load(root); }
    Word(WordId id, const Number& number, const std::string& str):
        id{id}, num{number}, str{str} {}

    auto dump() const -> save::blob override;
    void load(const save::blob&) override;

    friend std::ostream& operator<<(std::ostream&, const Word&);

    WordId      id;
    Number      num;
    std::string str;
};

class AdjMatrix:
    public std::unordered_map<WordId, std::set<WordId>>,
    public save::serializable
{
public:
    using std::unordered_map<WordId, std::set<WordId>>::unordered_map;
    AdjMatrix(const save::blob& root) { load(root); }

    auto dump() const -> save::blob override;
    void load(const save::blob&) override;
};

class DictEntry: public save::serializable {
public:
    DictEntry(const Word& w): word_{w} {}
    virtual ~DictEntry() = default;

    auto dump() const -> save::blob override { return word_.dump(); }
    void load(const save::blob& root) override { return word_.load(root); }
    auto word() const -> const Word& { return word_; }

private:
    Word    word_;
};

class Dict:
    public std::unordered_map<WordId, std::shared_ptr<DictEntry>>,
    public save::dumpable
{
public:
    using Idxstr = std::unordered_map<std::string, std::shared_ptr<DictEntry>>;
    Dict(const save::blob&);
    Dict(const Alphabet& ab): alphabet_{ab} {}
    virtual ~Dict() = default;

    auto dump() const -> save::blob override;

    void update(const std::string& textfile);
    void insert(const Word&);
    void consider_sentence(const std::string&);
    auto consider_word(const std::string&) -> WordId;
    auto idxstr() const -> const Idxstr& { return idxstr_; }
    auto adjmx() const -> const AdjMatrix& { return adjmx_; }

private:
    static const size_t max_file_size_ = 100*1024*1024;

    Alphabet alphabet_;
    AdjMatrix adjmx_;
    WordId last_id_{1};
    Idxstr idxstr_;
};

}
