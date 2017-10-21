#include "httpserver.hpp"
#include "helpers.hpp"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace HTTP;

Server::Server() {
    try {
        // creates logger
        log = spdlog::stdout_color_mt("httpserver");
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }

}

void Server::start(const std::string& path, uint32_t port) {

    // path to server's root
    char* root = ::realpath(path.c_str(), NULL);
    if (root == NULL) {
        log->error("Path " + path + " not found");
        throw std::runtime_error("Path " + path + " not found");
    };

    // ensure root is executable
    if (::access(root, X_OK) == -1) {
        log->error("Not allowed to execute on " + path);
        throw std::runtime_error("Not allowed to execute on " + path);
    }

    std::string root_path(root);
    free(root);

    try {
        log->info("Using " + root_path + " as path.");
        this->server_socket.bind(port);
        this->server_socket.listen();

        log->info("Listening on Port " + std::to_string(port));

        //std::thread t1(&Socket::TCPSocket::accept, &this->server_socket);
        std::thread t1([this] { this->accept(); });
        t1.detach();

    }
    catch (Socket::ConnectionException& er){

        log->error("Failed server start. \
                        Description: " + std::string(er.what()));

        throw std::runtime_error("Deu ruim no bind ou listen");
    }
}

void Server::accept() {
    while(1) {
        std::shared_ptr<Socket::TCPSocket> client_socket = this->server_socket.accept();
        std::thread t2([this, client_socket] { this->handle(client_socket); });
        t2.detach();
    }
}

void Server::handle(std::shared_ptr<Socket::TCPSocket> client_socket) {
    log->info("Connection established with IP " + client_socket->get_ip_address());
    try {
    while(1) {
        std::string request_str;
        HTTP::Request client_request;

        while (request_str.find("\r\n\r\n") == std::string::npos) {
            request_str += client_socket->recv(4096);
        }

        std::vector<std::string> request_lines = Helpers::split(request_str, "\r\n");
        std::vector<std::string> request_line = Helpers::split(request_lines[0], ' ');

        client_request.method       = request_line[0];
        client_request.uri          = request_line[1];
        client_request.http_version = request_line[2];

        for (int i = 1; i < request_lines.size(); i++) {
            std::vector<std::string> header_vector = Helpers::split(request_lines[i], ':');
            std::pair<std::string, std::string> header;
            header.first  = header_vector[0];
            header.second = header_vector[1];
            client_request.request_headers.push_back(header);
        }

        if (client_request.method != "GET") log->error("Not GET");
        else {
            response(client_request);
        }

    }
    }
    catch (std::exception& e) {
        log->error(e.what());
    }
}

void Server::response(Request req) {
    
}