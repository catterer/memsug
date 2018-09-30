#pragma once
#include <memory>
#include <stdexcept>
#include <evhttp.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <util.hh>

namespace server {

using Config = boost::property_tree::ptree;
using Evhttp = std::shared_ptr<evhttp>;

class Server {
public:
    Server(const std::string& config_path);
    void run();

private:
    Config cfg_;
    Evhttp evhttp_;
};

}
