#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>

int main(int argc, char** argv) try {
    if (!event_init())
        throw std::runtime_error("failed to init libevent");

    auto host = "127.0.0.1";
    auto port = 5555;
    std::unique_ptr<evhttp, decltype(&evhttp_free)> server(evhttp_start(host, port), &evhttp_free);

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

    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return EXIT_FAILURE;
}
