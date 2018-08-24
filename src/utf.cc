#include <utf.hh>
#include <stdexcept>

namespace utf {

auto split(const std::string& in)
    -> std::vector<std::string>
{
    std::vector<std::string> result;

    int char_size;
    for (auto cursor = in.data(); cursor < in.data() + in.size(); cursor += char_size) {
        if ((*cursor & 0x80) == 0)           char_size = 1;
        else if ((*cursor & 0xE0) == 0xC0)   char_size = 2;
        else if ((*cursor & 0xF0) == 0xE0)   char_size = 3;
        else if ((*cursor & 0xF8) == 0xF0)   char_size = 4;
        else throw std::invalid_argument("invalid utf8 header byte");

        for (int i = 1; i < char_size; i++)
            if ((cursor[i] & 0xC0) != 0x80)
                throw std::invalid_argument("invalid utf8 sequence");

        result.emplace_back(std::string(cursor, char_size));
    }

    return result;
}

}
