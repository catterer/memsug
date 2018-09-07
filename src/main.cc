#include <memsug.hh>
#include <valuer.hh>
#include <stdexcept>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void conflicting_options(const po::variables_map& vm,
                         const std::string& opt1, const std::string& opt2)
{
    if (vm.count(opt1) and !vm[opt1].defaulted() and
        vm.count(opt2) and !vm[opt2].defaulted())
        throw std::logic_error(std::string("Conflicting options '") +
                               opt1 + "' and '" + opt2 + "'.");
}

int main(int argc, char** argv) try {
    std::vector<std::string> textfiles;
    std::string alphabet_name;
    std::vector<std::string> numbers;
    std::string dictfile;

    po::options_description opts("Allowed options");
    opts.add_options()
        ("help,h", "print usage message")
        ("text,t", po::value(&textfiles), "path to file with text to build dictionary on")
        ("dict,d", po::value(&dictfile), "path to file with dictionary")
        ("alphabet,a", po::value(&alphabet_name), "alphabet to use: ru/en")
        ("number", po::value(&numbers), "number to decompose");

    po::positional_options_description posit;
    posit.add("number", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(opts).positional(posit).run(), vm);
    po::notify(vm);

    conflicting_options(vm, "text", "dict");

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
    for (const auto f: textfiles)
        dict.update(f, m);

    auto suger = memsug::Suger::create(std::move(dict));

    for (auto n: numbers) {
        valuer::Valuer valr(m);

        for (auto shorten = 0; ; shorten++) {
            auto res = suger->maximize_word_length(n, shorten);
            if (!res)
                break;
//            std::cout << "s" << shorten << ": ";
            for (const auto& v: *res)
                std::cout << suger->dict().at(v[0])->word().str << " ";
//            std::cout << "\n";

            valr.update(*res);
        }

        std::cout << n << ": ";
        auto sol = valr.solve();
        for (const auto& wid: sol)
            std::cout << suger->dict().at(wid)->word().str << " ";
        std::cout << "\n";
    }

    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return EXIT_FAILURE;
}
