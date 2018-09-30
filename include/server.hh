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
    Server(const std::string& config_path);
    void run();

private:
    Config cfg_;
    Evhttp evhttp_;
};

class Request {
public:
    Request(evhttp_request**);
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;
    virtual ~Request();

    void reply(const std::string& data);
    friend std::ostream& operator<<(std::ostream&, const Request&);

private:
    evhttp_request* evreq_;
};

}
