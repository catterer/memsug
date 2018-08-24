#include <text.hh>
#include <utf.hh>

namespace text {

Alphabet::Alphabet(std::vector<std::string> descriptor) {
    if (descriptor.size() != 10)
        throw std::invalid_argument("What are you?");

    for (preftree::Digit dig = 0; dig < descriptor.size(); ++dig)
        for (auto c: utf::split(descriptor[dig]))
            (*this)[c] = dig;
}

auto Alphabet::map(const std::string& word)
    -> optional<Key>
{
    Key key = 0;
    for (auto c: utf::split(word)) {
        auto pi = find(c);
        if (pi == end())
            continue;
        if (key * 10 < key)
            return {};

        key *= 10;
        key += pi->second;
    }

    return {key};
}

void Dict::update(const std::string& textfile) {
    auto f = std::shared_ptr<FILE>(::fopen(textfile.c_str(), "r"), [] (FILE* f) { if (f) fclose(f); });
    if (!f)
        throw std::runtime_error("Failed to open dict file");

    auto buf = std::shared_ptr<char>((char*)malloc(max_file_size_), [] (char* p) { free(p); });

    char* sp_s = NULL;
    const char* delim_s = ".?!;\"'";
    for (auto stnc = strtok_r(buf.get(), delim_s, &sp_s); stnc; stnc = strtok_r(NULL, delim_s, &sp_s))
        consider_sentence(stnc);
}

void Dict::consider_sentence(const std::string& stnc_) {
    char* sp_w = NULL;
    const char* delim_w = " \t,:-+=()";
    auto stnc = std::shared_ptr<char>(strdup(stnc_.c_str()), [] (char* p) { free(p); });
    for (auto w = strtok_r(stnc.get(), delim_w, &sp_w); w; w = strtok_r(NULL, delim_w, &sp_w))
        consider_word(w);
}

void Dict::consider_word(const std::string& w) {
    auto k = alphabet_.map(w);
    if (!k)
        return;
    std::string word_str(w);
    if (idxstr_.count(word_str))
        return;

    return insert(Word{last_id_++, *k, w});
}

void Dict::insert(Word&& w) {
    auto entry = std::make_shared<DictEntry>(std::move(w));
    emplace(entry->word().id, entry);
    idxstr_.emplace(entry->word().str, entry);
}

}
