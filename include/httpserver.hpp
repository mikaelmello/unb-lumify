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

struct RequestError {
    RequestError(uint16_t code) : code(code) {}
    uint16_t code;
};

class Request {
    public:
        std::string method; 
        std::string file_path;
        std::string absolute_path;
        std::string http_version;

        std::vector<std::string> uri;
        std::vector<std::pair<std::string, std::string>> request_headers;
        std::vector<std::pair<std::string, std::string>> uri_parameters;
};

class Server {

    public:

        Server();

        void start(const std::string& path, uint32_t port = 8080);

        void stop();

    private:

        void accept();

        void handle_request(std::shared_ptr<Socket::TCPSocket> client_socket);

        void handle_response(std::shared_ptr<Socket::TCPSocket> client_socket, 
                             HTTP::Request client_request);

        void transfer(std::shared_ptr<Socket::TCPSocket> client_socket, 
                      HTTP::Request client_request, const std::string& file_type);

        void interpret(std::shared_ptr<Socket::TCPSocket> client_socket, 
                       HTTP::Request client_request);

        void respond(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code, 
                     const std::vector<std::pair<std::string, std::string>>& headers, 
                     uint8_t * body, uint32_t length);

        void error(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code);

        std::string reason(uint16_t code);

        std::string url_decode(const std::string& url);

        std::string lookup_type(const std::string& extension);

        std::shared_ptr<spdlog::logger> log;

        Socket::TCPSocket server_socket;

        std::string root_path;

};

} // end namespace HTTP

#endif