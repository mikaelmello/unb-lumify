#ifndef HTTPSERVER_H
#define HTTPSERVER_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdexcept>
#include <string>
#include <utility>
#include <memory>
#include "spdlog/spdlog.h"

#include "sockets.hpp"

namespace HTTP {

class Request {
    public:
        std::string method; 
        std::string uri;
        std::string http_version;

        std::vector<std::pair<std::string, std::string>> request_headers;
};

class Server {

    public:

        Server();

        void start(const std::string& path, uint32_t port = 8080);

        void stop();

    private:

        void accept();

        void handle(std::shared_ptr<Socket::TCPSocket> client_socket);

        void response(HTTP::Request req);

        std::shared_ptr<spdlog::logger> log;

        Socket::TCPSocket server_socket;

};

} // end namespace HTTP

#endif