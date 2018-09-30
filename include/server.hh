#pragma once
#include <memory>
#include <evhttp.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace server {

using Config = boost::property_tree::ptree;
using Evhttp = std::shared_ptr<evhttp>;

class Server {
public:
    Server(const std::string& config_path)
    {
        boost::property_tree::ini_parser::read_ini(config_path, cfg_);
        if (!event_init())
            throw std::runtime_error("failed to init libevent");

        auto host = cfg_.get("bind.host", "localhost");
        auto port = cfg_.get("bind.port", 8080);
        evhttp_ = std::shared_ptr<evhttp>(evhttp_start(host.c_str(), port), evhttp_free);
        if (!evhttp_)
            throw std::runtime_error("failed to init http server");
    }

    void run();

private:
    Config cfg_;
    Evhttp evhttp_;
};

}
