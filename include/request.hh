#pragma once
#include <string>
#include <map>
#include <functional>
#include <evhttp.h>

#include <util.hh>

namespace request {

class Uri {
public:
    Uri(const char*);
    auto path() const -> std::string;
private:
    std::shared_ptr<evhttp_uri> evuri_{};
};

class Request:
    public util::NonCopyable,
    public std::enable_shared_from_this<Request>
{
public:
    Request(Uri&&, evhttp_request**);
    Request& operator=(const Request&) = delete;
    virtual ~Request();

    void reply(const std::string& data);
    virtual void process() = 0;
    friend std::ostream& operator<<(std::ostream&, const Request&);

private:
    evhttp_request* evreq_;
    Uri uri_;
};

using PathProcessorGenerator = std::function<std::shared_ptr<Request> (Uri&&, evhttp_request** rq)>;

class RequestFactory:
    public util::Singleton<RequestFactory>,
    public std::map<std::string, PathProcessorGenerator>
{
public:
    RequestFactory();
    auto assemble(Uri&&, evhttp_request**) -> std::shared_ptr<Request>;
};

class Suggest: public Request {
public:
    using Request::Request;
    void process() override;
};

}
