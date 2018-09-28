#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using Config = boost::property_tree::ptree;

class Server {
public:
    Server(const std::string& config_path)
    {
        boost::property_tree::ini_parser::read_ini(config_path, cfg_);
    }

    void run();

private:
    Config cfg_;
};

void Server::run() {
    if (!event_init())
        throw std::runtime_error("failed to init libevent");

    auto host = cfg_.get("bind.host", "localhost");
    auto port = cfg_.get("bind.port", 8080);
    std::cout << host << ":" << port << "\n";
    std::unique_ptr<evhttp, decltype(&evhttp_free)> server(evhttp_start(host.c_str(), port), &evhttp_free);

    if (!server)
        throw std::runtime_error("failed to init http server");

    evhttp_set_gencb(server.get(), [] (evhttp_request *req, void *) {
            auto *reply = evhttp_request_get_output_buffer(req);
            if (!reply)
                return;
            evbuffer_add_printf(reply, "<html><body><center><h1>Hello World!</h1></center></body></html>");
            evhttp_send_reply(req, HTTP_OK, "", reply);
        } , nullptr);

    if (event_dispatch() == -1)
        throw std::runtime_error("failed to run event loop");
}

int main(int argc, char** argv) try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", po::value<std::string>(), "config file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (not vm.count("config")) {
        std::cout << "no config specified\n";
        std::cout << desc << "\n";
        return 1;
    }

    Server srv(vm["config"].as<std::string>());
    srv.run();
    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return EXIT_FAILURE;
}
