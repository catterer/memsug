#include <request.hh>
#include <log.hh>

namespace request {

RequestFactory::RequestFactory() {
    emplace("/suggest",
            [] (auto&& uri, auto&& rq) { return std::make_shared<Suggest>(std::move(uri), rq); });
}

auto RequestFactory::assemble(Uri&& uri, evhttp_request** rq)
    -> std::shared_ptr<Request>
{
    auto i = find(uri.path());
    if (i == end())
        return {};
    return i->second(std::move(uri), rq);
}

void Suggest::process() {
    return reply("yaaay");
}

Request::Request(Uri&& uri, evhttp_request** r):
    evreq_{*r},
    uri_{std::move(uri)}
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

Uri::Uri(const char* uri):
    evuri_{std::shared_ptr<evhttp_uri>(evhttp_uri_parse(uri), evhttp_uri_free)}
{
    if (not evuri_)
        throw std::invalid_argument("invalid uri format");
}

auto Uri::path() const
    -> std::string
{
    auto p = evhttp_uri_get_path(evuri_.get());
    if (!p)
        throw std::logic_error("couldn't get path from the uri");
    return {p};
}


}
