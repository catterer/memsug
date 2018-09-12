#include <memsug.hh>
#include <valuer.hh>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>

namespace po = boost::program_options;

using Cb = std::function<void(int, char**)>;

void help(int, char**);
void suggest(int, char**);
void build(int, char**);
void update(int, char**);

static const std::map<std::string, Cb> handlers = {
    {"help", help},
    {"suggest", suggest},
    {"build", build},
    {"update", update},
};

void conflicting_options(const po::variables_map& vm,
                         const std::string& opt1, const std::string& opt2)
{
    if (vm.count(opt1) and !vm[opt1].defaulted() and
        vm.count(opt2) and !vm[opt2].defaulted())
        throw std::logic_error(std::string("Conflicting options '") +
                               opt1 + "' and '" + opt2 + "'.");
}

void required_options(const po::variables_map& vm, const std::list<std::string> vars) {
    for (const auto& n: vars)
        if (!vm.count(n))
            throw po::required_option(n);
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
        ("number,n", po::value(&numbers), "number to decompose")
        ("more,m", "more options");

    po::positional_options_description posit;
    posit.add("number", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(opts).positional(posit).run(), vm);
    po::notify(vm);

    conflicting_options(vm, "text", "dict");
    conflicting_options(vm, "alphabet", "dict");

    if (vm.count("help")) {
        std::cout << opts << "\n";
        return;
    }

    required_options(vm, {"number"});

    std::unique_ptr<text::Dict> dict{};

    if (vm.count("dict")) {
        save::blob blob;
        boost::property_tree::read_json(dictfile, blob);
        dict = std::make_unique<text::Dict>(blob);
    } else if (vm.count("text")) {
        if (!vm.count("alphabet"))
            throw std::invalid_argument("alphabet not specified");
        dict = std::make_unique<text::Dict>(text::Alphabet::by_name(alphabet_name));
        for (const auto f: textfiles)
            dict->update(f);
    } else
        throw std::invalid_argument("you must specify dictionary somehow");

    auto suger = memsug::Suger::create(std::move(*dict));
    dict = {};

    for (auto n: numbers) {
        valuer::Valuer valr(suger->dict().adjmx());

        for (auto shorten = 0; ; shorten++) {
            auto res = suger->maximize_word_length(n, shorten);
            if (!res)
                break;
            if (vm.count("more")) {
                for (const auto& sm: *res) {
                    for (const auto& wid: sm)
                        std::cout << suger->dict().at(wid)->word().str << " ";
                    std::cout << "\n";
                }
            }
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

void build(int argc, char** argv) {
    std::vector<std::string> textfiles;
    std::string alphabet_name;
    std::string dictfile;

    po::options_description opts("Allowed options");
    opts.add_options()
        ("help,h", "print usage message")
        ("text,t", po::value(&textfiles), "path to file with text to build dictionary on")
        ("alphabet,a", po::value(&alphabet_name), "alphabet to use: ru/en")
        ("out,o", po::value(&dictfile), "path to output file");

    po::positional_options_description posit;
    posit.add("text", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(opts).positional(posit).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << "\n";
        return;
    }

    required_options(vm, {"text", "alphabet", "out"});

    text::Dict dict(text::Alphabet::by_name(alphabet_name));
    for (const auto f: textfiles)
        dict.update(f);

    boost::property_tree::write_json(dictfile, dict.dump());
}

void update(int argc, char** argv) {
    std::vector<std::string> textfiles;
    std::string dictfile;

    po::options_description opts("Allowed options");
    opts.add_options()
        ("help,h", "print usage message")
        ("text,t", po::value(&textfiles), "path to file with text to use for dictionary update")
        ("dict,d", po::value(&dictfile), "path to dictionary file to update");

    po::positional_options_description posit;
    posit.add("text", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(opts).positional(posit).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << "\n";
        return;
    }

    required_options(vm, {"text", "dict"});

    save::blob blob;
    boost::property_tree::read_json(dictfile, blob);
    text::Dict dict(blob);

    for (const auto f: textfiles)
        dict.update(f);

    boost::property_tree::write_json(dictfile, dict.dump());
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
