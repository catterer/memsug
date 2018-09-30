#include <server.hh>
#include <request.hh>
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
    evhttp_set_gencb(evhttp_.get(), [] (evhttp_request *evreq, void *) {
            try {
                auto uri = request::Uri(evhttp_request_get_uri(evreq));

                Log(debug) << "New event " << uri.path();

                auto r = request::RequestFactory::get().assemble(std::move(uri), &evreq);
                if (!r)
                    return evhttp_send_error(evreq, HTTP_NOTFOUND, "Not found");

                Log(warn) << "New request " << *r;

                return r->process();
            } catch(const std::exception& x) {
                Log(error) << "Exception caught: " << x.what();
                abort();
            }
        } , nullptr);

    if (event_dispatch() == -1)
        throw std::runtime_error("failed to run event loop");
}

}
