#include <server.hh>

#include <iostream>
#include <log.hh>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv) try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", po::value<std::string>(), "config file")
        ("log-level,l", po::value<int>()->default_value(2), "log level (0-4)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    log::Logger::get().filter((log::Level)vm["log-level"].as<int>());

    if (not vm.count("config")) {
        std::cout << "no config specified\n";
        std::cout << desc << "\n";
        return 1;
    }

    server::Server srv(vm["config"].as<std::string>());
    srv.run();
    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return EXIT_FAILURE;
}
