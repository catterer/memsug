#include <memsug.hh>
#include <valuer.hh>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <map>

namespace po = boost::program_options;

using Cb = std::function<void(int, char**)>;

void help(int, char**);
void suggest(int, char**);

static const std::map<std::string, Cb> handlers = {
    {"help", help},
    {"suggest", suggest}
};

void conflicting_options(const po::variables_map& vm,
                         const std::string& opt1, const std::string& opt2)
{
    if (vm.count(opt1) and !vm[opt1].defaulted() and
        vm.count(opt2) and !vm[opt2].defaulted())
        throw std::logic_error(std::string("Conflicting options '") +
                               opt1 + "' and '" + opt2 + "'.");
}

void suggest(int argc, char** argv) {
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
        return;
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
            valr.update(*res);
        }

        std::cout << n << ": ";
        auto sol = valr.solve();
        for (const auto& wid: sol)
            std::cout << suger->dict().at(wid)->word().str << " ";
        std::cout << "\n";
    }
}

void help(int, char**) {
    std::cout << "Available subcommands: \n";
    for (const auto& h: handlers)
        std::cout << "    " << h.first << "\n";
}

int main(int argc, char** argv) try {
    if (argc < 2) {
        help(0, NULL);
        return 0;
    }

    std::string method(argv[1]);

    if (!handlers.count(method)) {
        help(0, NULL);
        return 0;
    }

    handlers.at(method)(argc-1, argv+1);
    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return EXIT_FAILURE;
}
