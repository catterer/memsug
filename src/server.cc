#include <server.hh>
#include <log.hh>

namespace server {

Server::Server(const std::string& config_path) {
    boost::property_tree::ini_parser::read_ini(config_path, cfg_);
    if (!event_init())
        throw std::runtime_error("failed to init libevent");

    auto host = cfg_.get("bind.host", "localhost");
    auto port = cfg_.get("bind.port", 8080);
    evhttp_ = std::shared_ptr<evhttp>(evhttp_start(host.c_str(), port), evhttp_free);
    if (!evhttp_)
        throw std::runtime_error("failed to init http server");
}

void Server::run() {
    evhttp_set_gencb(evhttp_.get(), [] (evhttp_request *req, void *) {
            auto r = std::make_shared<Request>(&req);
            return r->reply("<html><body><center><h1>Hello World!</h1></center></body></html>");
        } , nullptr);

    if (event_dispatch() == -1)
        throw std::runtime_error("failed to run event loop");
}

Request::Request(evhttp_request** r):
    evreq_{*r}
{
    *r = NULL;
    Log(info) << "New request " << *this;
}

std::ostream& operator<<(std::ostream& out, const Request& r) {
    out << evhttp_request_get_uri(r.evreq_);
    return out;
}

void Request::reply(const std::string& data) {
    auto *buf = evhttp_request_get_output_buffer(evreq_);
    evbuffer_add(buf, data.c_str(), data.size());
    evhttp_send_reply(evreq_, HTTP_OK, "", buf);
    evreq_ = NULL;
}

Request::~Request() {
    if (evreq_) {
        Log(error) << "Request not replied: " << *this;
        evhttp_send_error(evreq_, HTTP_INTERNAL, "No reply from server");
    }
}

}
