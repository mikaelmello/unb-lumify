#include "httpserver.hpp"
#include "helpers.hpp"
#include <iostream>
#include <fstream>
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

    this->root_path = root;
    free(root);

    try {
        log->info("Using " + this->root_path + " as path.");
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
        std::thread t2([this, client_socket] { this->handle_request(client_socket); });
        t2.detach();
    }
}

void Server::handle_request(std::shared_ptr<Socket::TCPSocket> client_socket) {
    try {
        std::string request_str;
        HTTP::Request client_request;

        while (request_str.find("\r\n\r\n") == std::string::npos) {
            request_str += client_socket->recv(4096);
        }

        std::vector<std::string> request_lines = Helpers::split(request_str, "\r\n");
        std::vector<std::string> request_line = Helpers::split(request_lines[0], ' ');

        client_request.method        = request_line[0];
        client_request.uri           = Helpers::split(request_line[1], '?');
        client_request.file_path     = client_request.uri[0];
        client_request.absolute_path = this->root_path + client_request.uri[0];
        client_request.http_version  = request_line[2];

        if (client_request.uri.size() > 1) {
            std::vector<std::string> parameters = Helpers::split(client_request.uri[1], '&');
            for (auto prmtr : parameters) {
                std::vector<std::string> key_value = Helpers::split(prmtr, '=');

                std::pair<std::string, std::string> single_parameter(key_value[0], key_value[1]);

                client_request.uri_parameters.push_back(single_parameter);
            }
        }

        for (int i = 1; i < request_lines.size(); i++) {
            std::vector<std::string> header_vector = Helpers::split(request_lines[i], ':');
            if (header_vector.size() != 2) continue;
            std::pair<std::string, std::string> header;
            header.first  = header_vector[0];
            header.second = header_vector[1];
            client_request.request_headers.push_back(header);
        }

        try {
            log->info(request_lines[0] + " from " + client_socket->get_ip_address());
            handle_response(client_socket, client_request);            
        }
        catch (const RequestError& e) {
            error(client_socket, e.code);
        }

    }
    catch (std::exception& e) {
        log->error(e.what());
    }
}

void Server::handle_response(std::shared_ptr<Socket::TCPSocket> client_socket, Request client_request) {
    std::string file_path = client_request.file_path;
    std::string absolute_path = client_request.absolute_path;
    std::string file_extension = file_path.substr(file_path.find_last_of(".") + 1);
    std::string file_type = lookup_type(file_extension);

    if (client_request.method != "GET") throw RequestError(405);
    if (file_path[0] != '/') throw RequestError(501);
    if (file_path.find('"') != file_path.npos) throw RequestError(400);

    if (file_path == "/") {
        file_path = "/index.php";
        file_extension = "php";
    }
    // ensure path is readable
    if (access(absolute_path.c_str(), R_OK) == -1) throw RequestError(404);

    if (file_extension == "php") interpret(client_socket, client_request);
    else transfer(client_socket, client_request, file_type);

}

void Server::transfer(std::shared_ptr<Socket::TCPSocket> client_socket, 
                      Request client_request, const std::string& file_type) {

    std::ifstream file_requested;

    file_requested.open(client_request.absolute_path, std::ifstream::ate | std::ifstream::binary);
    int file_size = file_requested.tellg();

    file_requested.close();
    file_requested.open(client_request.absolute_path, std::ifstream::in | std::ifstream::binary);

    uint8_t buffer[file_size];
    file_requested.read((char *) buffer, file_size);
    if (!file_requested) throw RequestError(500);

    std::vector<std::pair<std::string, std::string>> headers;
    if (file_type != "") {
        headers.push_back(std::pair<std::string, std::string>("Content-Type", file_type));
    }

    respond(client_socket, 200, headers, buffer, file_size);
}

void Server::interpret(std::shared_ptr<Socket::TCPSocket> client_socket, Request client_request) {

}

void Server::respond(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code, 
                     const std::vector<std::pair<std::string, std::string>>& headers, 
                     uint8_t * body, uint32_t length) {

    std::string response = "HTTP/1.1 " + std::to_string(code) + " " + reason(code) + "\r\n";
    for (auto header : headers) {
        response += header.first + ": " + header.second + "\r\n";
    }
    response += "Content-Length: " + std::to_string(length) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";

    if (code < 300) log->info("HTTP/1.1 " + std::to_string(code) + " " + reason(code));
    else            log->error("HTTP/1.1 " + std::to_string(code) + " " + reason(code));
    client_socket->send(response);
    client_socket->send(body, length);

}

void Server::error(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code) {

    std::vector<std::pair<std::string, std::string>> headers;
    std::pair<std::string, std::string> content_type("Content-Type", "text/html");
    headers.push_back(content_type);

    std::string body = "<html><head><title>"       + std::to_string(code) + " " + reason(code) + 
                       "</title></head><body><h1>" + std::to_string(code) + " " + reason(code) +
                       "</h1></body></html>";

    respond(client_socket, code, headers, (uint8_t *) body.c_str(), body.length());
}

std::string Server::reason(uint16_t code) {
    switch (code)
    {
        case 200: return "OK";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 414: return "Request-URI Too Long";
        case 418: return "I'm a teapot";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        default:  return "None";
    }
}


std::string Server::url_decode(const std::string& url) {

    std::string decoded;

    // Decodificando octetos codificados por %
    // https://www.ietf.org/rfc/rfc3986.txt
    for (int i = 0, n = url.size(); i < n; i++) {
        if (url[i] == '%' && i < n - 2) {
            std::string octet = url.substr(i + 1, 2);
            decoded += (char) std::stol(octet, NULL, 16);
            i += 2;
        }
        else if (url[i] == '+') {
            decoded += ' ';
        }
        else {
            decoded += url[i];
        }
    }

    return decoded;
}

std::string Server::lookup_type(const std::string& extension) {
    if (extension == "css")  return "text/css";
    if (extension == "html") return "text/html";       
    if (extension == "gif")  return "image/gif";       
    if (extension == "ico")  return "image/x-icon";    
    if (extension == "jpg")  return "image/jpeg";      
    if (extension == "js")   return "text/javascript"; 
    if (extension == "php")  return "text/x-php";      
    if (extension == "png")  return "image/png";       

    return "";
}