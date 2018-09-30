#include <server.hh>

namespace server {

void Server::run() {
    evhttp_set_gencb(evhttp_.get(), [] (evhttp_request *req, void *) {
            auto *reply = evhttp_request_get_output_buffer(req);
            if (!reply)
                return;
            evbuffer_add_printf(reply, "<html><body><center><h1>Hello World!</h1></center></body></html>");
            evhttp_send_reply(req, HTTP_OK, "", reply);
        } , nullptr);

    if (event_dispatch() == -1)
        throw std::runtime_error("failed to run event loop");
}

}
