#include <text.hh>
#include <utf.hh>
#include <optional.hh>
#include <locale>

#include <boost/lexical_cast.hpp>

namespace text {

auto Alphabet::classic_ru()
    -> Alphabet
{
    return Alphabet({
                "ЗСзс",
                "ДТдт",
                "Нн",
                "Мм",
                "Чч",
                "Лл",
                "ЖШЩжшщ",
                "КГкг",
                "ФВфв",
                "РПБрпб"
            });
}

auto Alphabet::classic_en()
    -> Alphabet
{
    return Alphabet({
                "Oo",
                "Aa",
                "Bb",
                "Cc",
                "Dd",
                "Ee",
                "Ss",
                "Gg",
                "Hh",
                "Nn"
            });
}

auto Alphabet::by_name(const std::string& n)
    -> Alphabet
{
    if (n == "ru")
        return Alphabet::classic_ru();
    if (n == "en")
        return Alphabet::classic_en();
    throw std::runtime_error("Unknown alphabet name");
}

Alphabet::Alphabet(std::vector<std::string>&& descriptor) {
    if (descriptor.size() != 10)
        throw std::invalid_argument("What are you?");

    for (uint8_t dig = 0; dig < descriptor.size(); ++dig)
        for (auto c: utf::split(descriptor[dig]))
            (*this)[c] = dig;
}

auto Alphabet::dump() const
    -> save::blob
{
    save::blob mapping{};
    for (const auto& p: *this)
        mapping.put(p.first, p.second);

    save::blob res;
    res.add_child("mapping", std::move(mapping));
    return res;
}

void Alphabet::load(const save::blob& in) {
    const auto& mapping = in.get_child("mapping");
    for (const auto& p: mapping)
        emplace(p.first, mapping.get<int>(p.first));
}

auto Alphabet::map(const std::string& word) const
    -> Number
{
    Number num;
    for (auto c: utf::split(word)) {
        auto pi = find(c);
        if (pi == end())
            continue;

        num += 0x30 + pi->second;
    }

    return num;
}

auto Word::dump() const
    -> save::blob
{
    save::blob res{};
    res.put("id", id);
    res.put("num", num);
    res.put("str", str);
    return res;
}

void Word::load(const save::blob& root) {
    id = root.get<WordId>("id");
    num = root.get<Number>("num");
    str = root.get<std::string>("str");
}

std::ostream& operator<<(std::ostream& out, const Word& w) {
    out << w.id << ":" << w.str << "=" << w.num;
    return out;
}

auto Dict::dump() const
    -> save::blob
{
    save::blob res{};

    save::blob entries{};
    for (const auto& p: *this)
        entries.add_child(std::to_string(p.first), p.second->dump());
    res.add_child("entries", std::move(entries));

    return res;
}

void Dict::load(const save::blob& root) {
    for (const auto& p: root.get_child("entries"))
        insert(Word(p.second));
}

void Dict::update(const std::string& textfile, AdjMatrix& m) {
    auto f = std::shared_ptr<FILE>(::fopen(textfile.c_str(), "r"), [] (FILE* f) { if (f) fclose(f); });
    if (!f)
        throw std::runtime_error("Failed to open dict file");

    auto buf = std::shared_ptr<char>((char*)malloc(max_file_size_), [] (char* p) { free(p); });
    ::fread(buf.get(), 1, max_file_size_, f.get());
    if (ferror(f.get()))
        throw std::runtime_error("Failed to read file");
    if (!feof(f.get()))
        throw std::runtime_error("File too large");

    char* sp_s = NULL;
    const char* delim_s = ".?!;\"',:()";
    for (auto stnc = strtok_r(buf.get(), delim_s, &sp_s); stnc; stnc = strtok_r(NULL, delim_s, &sp_s))
        consider_sentence(stnc, m);
}

void Dict::consider_sentence(const std::string& stnc_, AdjMatrix& m) {
    char* sp_w = NULL;
    const char* delim_w = " \t\n\r+=-";
    auto stnc = std::shared_ptr<char>(strdup(stnc_.c_str()), [] (char* p) { free(p); });
    optional<WordId> prev_id;
    for (auto w = strtok_r(stnc.get(), delim_w, &sp_w); w; w = strtok_r(NULL, delim_w, &sp_w)) {
        std::string word = w;
        auto new_id = consider_word(word);
        if (prev_id)
            m[*prev_id].emplace(new_id);
        prev_id.emplace(new_id);
    }
}

WordId Dict::consider_word(const std::string& w) {
    auto i = idxstr_.find(w);
    if (i != idxstr_.end())
        return i->second->word().id;

    insert(Word{last_id_, alphabet_.map(w), w});
    return last_id_++;
}

void Dict::insert(Word&& w) {
    auto entry = std::make_shared<DictEntry>(std::move(w));
    emplace(entry->word().id, entry);
    idxstr_.emplace(entry->word().str, entry);
}

}
