#include <memsug.hh>
#include <stdexcept>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv) try {
    std::string textfile;
    std::string alphabet_name;
    std::vector<std::string> numbers;

    po::options_description opts("Allowed options");
    opts.add_options()
        ("help,h", "print usage message")
        ("text,t", po::value(&textfile), "path to file with text to build dictionary on")
        ("alphabet,a", po::value(&alphabet_name), "alphabet to use: ru/en")
        ("number", po::value(&numbers), "input file");

    po::positional_options_description posit;
    posit.add("number", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(opts).positional(posit).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {  
        std::cout << opts << "\n";
        return 0;
    }

    if (!vm.count("text"))
        throw std::logic_error("text file not specified");

    if (!vm.count("number"))
        throw std::logic_error("number not specified");

    if (!vm.count("alphabet"))
        throw std::logic_error("alphabet not specified");

    text::AdjMatrix m;
    text::Dict dict(text::Alphabet::by_name(alphabet_name));
    dict.update(textfile, m);

    auto suger = memsug::Suger::create(std::move(dict));

    for (auto n: numbers) {
        std::cout << n << ":\n";
        auto res = suger->maximize_word_length(n);
        if (!res) {
            std::cout << "nothing found\n";
            continue;
        }

        for (const auto& v: *res) {
            for (const auto& w: v)
                std::cout << w << " ";
            std::cout << "\n";
        }
    }

    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return EXIT_FAILURE;
}
