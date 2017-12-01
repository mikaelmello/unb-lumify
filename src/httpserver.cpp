#include "httpserver.hpp"
#include "helpers.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace HTTP;

Server::Server() : is_finished(false), threads_qty(0) {
    try {
        // inicializa o logger
        log = spdlog::stdout_color_mt("httpserver");
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }

}

void Server::start(const std::string& path, uint32_t port) {

    // Path absoluto da root do servidor
    char* root = ::realpath(path.c_str(), NULL);
    if (root == NULL) {
        log->error("Path " + path + " not found");
        throw std::runtime_error("Path " + path + " not found");
    };

    // Garante que a root é executável
    if (::access(root, X_OK) == -1) {
        log->error("Not allowed to execute on " + path);
        throw std::runtime_error("Not allowed to execute on " + path);
    }

    // Coloca o caminho da root na variável membro da classe
    this->root_path = root;
    this->port = port;
    free(root);


    try {
        // Inicializa o servidor
        log->info("Using " + this->root_path + " as path.");
        this->server_socket.bind(port);
        this->server_socket.listen();

        log->info("Listening on Port " + std::to_string(port));

        // Aceita as conexões em um thread separado e 
        // volta pra função que chamou start()
        std::thread t1([this] { this->accept(); });
        t1.detach();
        this->threads_qty++;

    }
    catch (Socket::ConnectionException& er){

        log->error("Failed server start. \
                        Description: " + std::string(er.what()));

        throw std::runtime_error(er.what());
    }
}

void Server::stop() {
    // Seta a variável lida por accept() em seu loop
    log->warn("Initiating termination of HTTP Server");
    this->is_finished = true;

    // Encerra o socket do servidor
    log->warn("Closing server socket");
    this->server_socket.close();

    // Inicia uma conexão com o próprio servidor para finalizar
    // a chamada blocante de server_socket.accept() em accept()
    log->warn("Terminating accept() of new connections");
    Socket::TCPSocket a;
    a.connect("localhost", this->port);

    // Esperando todos os threads do servidor finalizarem
    log->warn("Waiting for all threads to finish");
    while(this->threads_qty > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    log->warn("Exit.");
}

void Server::accept() {
    // Aceita conexões indefinidamente enquanto o fim não é sinalizado
    while(!this->is_finished) {
        try {
            // Cria um ponteiro compartilhado que é uma socket do cliente
            // retornada por server_socket.accept();
            std::shared_ptr<Socket::TCPSocket> client_socket = this->server_socket.accept();
            
            // Define um timeout para receber dados do cliente.
            // Se o servidor já tiver sido finalizado, define-se
            // 1 segundo, 10 caso contrário.
            uint16_t client_timeout = (this->is_finished ? 1 : 10);
            client_socket->set_timeout(client_timeout);
            
            // Inicia um thread para lidar com o request do cliente e então
            // separa ele do thread atual, de modo a aceitar mais conexõs
            std::thread t2([this, client_socket] { this->handle_request(client_socket); });
            t2.detach();
            this->threads_qty++;
        }
        catch (const Socket::ConnectionException& e) {}
    }
    log->warn("Thread with accept() terminated.");
    this->threads_qty--;
}

void Server::handle_request(std::shared_ptr<Socket::TCPSocket> client_socket) {
    try {
        std::string request_str;
        HTTP::Request client_request;

        // Recebe dados até que seja encontrado CRLFCRLF (fim de um request).
        while (request_str.find("\r\n\r\n") == std::string::npos) {
            request_str += client_socket->recv(4096);
        }

        // As próximas 30 linhas preenchem um struct Request de acordo
        // com a string recebida do cliente
        std::vector<std::string> request_lines = Helpers::split(request_str, "\r\n");
        std::vector<std::string> request_line = Helpers::split(request_lines[0], ' ');

        request_line[1] = url_decode(request_line[1]);

        client_request.method        = request_line[0];
        client_request.uri           = Helpers::split(request_line[1], '?');
        client_request.file_path     = (client_request.uri[0] == "/" ? "/index.php" : client_request.uri[0]);
        client_request.absolute_path = this->root_path + client_request.file_path;
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

        // Resolve como responder ao request do cliente.
        try {
            log->info(request_lines[0] + " from " + client_socket->get_ip_address());
            handle_response(client_socket, client_request);            
        }
        catch (const RequestError& e) {
            error(client_socket, e.code);
        }

    }
    catch (std::exception& e) {
        // quick hack to not log timeout error of our own last connected socket
        if (!this->is_finished) {
            log->error("Error with user " + client_socket->get_ip_address() + ":");
            log->error(e.what());
        }
    }
    client_socket->close();
    this->threads_qty--;
}

void Server::handle_response(std::shared_ptr<Socket::TCPSocket> client_socket, Request client_request) {
    std::string file_path = client_request.file_path;
    std::string absolute_path = client_request.absolute_path;
    std::string file_extension = file_path.substr(file_path.find_last_of(".") + 1);
    std::string file_type = lookup_type(file_extension);

    // Único método suportado é o GET
    if (client_request.method != "GET") throw RequestError(405);

    // Erro de caminho inválido
    if (file_path[0] != '/') throw RequestError(501);

    // Recusa caminhos com aspas em seus caracteres
    if (file_path.find('"') != file_path.npos) throw RequestError(400);

    // Caso não seja possível acessar o arquivo.
    if (access(absolute_path.c_str(), R_OK) == -1) throw RequestError(404);

    // Se o arquivo for .php, ele é interpretado antes de transferido.
    if (file_type == "text/x-php") interpret(client_socket, client_request);
    else transfer(client_socket, client_request, file_type);

}

void Server::transfer(std::shared_ptr<Socket::TCPSocket> client_socket, 
                      Request client_request, const std::string& file_type) {

    std::ifstream file_requested;

    // Abre o arquivo, verifica o tamanho dele e fecha.
    file_requested.open(client_request.absolute_path, std::ifstream::ate | std::ifstream::binary);
    int file_size = file_requested.tellg();
    file_requested.close();

    // Abre novamente o arquivo, inicia um buffer com o tamanho necessário na memória
    file_requested.open(client_request.absolute_path, std::ifstream::in | std::ifstream::binary);

    uint8_t buffer[file_size];

    file_requested.read((char *) buffer, file_size);
    // Se um erro acontecer, um erro é lançado.
    if (!file_requested) throw RequestError(500);

    // Inicializa os headers da resposta e envia a resposta.
    std::vector<std::pair<std::string, std::string>> headers;
    if (file_type != "") {
        headers.push_back(std::pair<std::string, std::string>("Content-Type", file_type));
    }

    respond(client_socket, 200, headers, buffer, file_size);
}

void Server::interpret(std::shared_ptr<Socket::TCPSocket> client_socket, Request client_request) {
    char* path = new char [client_request.absolute_path.length()+1];
    std::strcpy (path, client_request.absolute_path.c_str());
    if (client_request.uri.size() == 1) {
        client_request.uri.push_back("");
    }
    char* query = new char [client_request.uri[1].length()+1];
    std::strcpy (query, client_request.uri[1].c_str());
    // ensure path is readable
    if (access(path, R_OK) == -1) {
        error(client_socket, 403);
        return;
    }

    // open pipe to PHP interpreter
    char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\"%s\" php-cgi";
    char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
    if (sprintf(command, format, query, path) < 0) {
        error(client_socket, 500);
        return;
    }
    FILE* file = popen(command, "r");
    if (file == NULL) {
        error(client_socket, 500);
        return;
    }

    // load interpreter's content
    char* content;
    size_t length;
    if (load(file, (uint8_t**) &content, &length) == false) {
        error(client_socket, 500);
        return;
    }

    // close pipe
    pclose(file);

    // subtract php-cgi's headers from content's length to get body's length
    char* haystack = content;
    char* needle = strstr(haystack, "\r\n\r\n");
    if (needle == NULL) {
        free(content);
        error(client_socket, 500);
        return;
    }

    // extract headers
    char header[needle + 2 - haystack + 1];
    strncpy(header, content, needle + 2 - haystack);
    header[needle + 2 - haystack] = '\0';

    std::vector<std::pair<std::string, std::string>> headers;

    std::vector<std::string> pre_headers = Helpers::split(header, "\r\n");
    for (auto s : pre_headers) {
        std::vector<std::string> key_value = Helpers::split(s, '=');
        std::pair<std::string, std::string> single_parameter(key_value[0], key_value[1]);
        headers.push_back(single_parameter);
    }
    headers.push_back(std::pair<std::string, std::string>("Content-Type", "text/html; charset=\"utf-8\""));
    // respond with interpreter's content
    respond(client_socket, 200, headers, (uint8_t*) needle + 4, length - (needle - haystack + 4));

    // free interpreter's content
    free(content);
    free(path);
}

void Server::respond(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code, 
                     const std::vector<std::pair<std::string, std::string>>& headers, 
                     uint8_t * body, uint32_t length) {

    // Simples construção de uma resposta HTTP.
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

    // Envia uma resposta com código e mensagem de erro.
    std::vector<std::pair<std::string, std::string>> headers;
    std::pair<std::string, std::string> content_type("Content-Type", "text/html");
    headers.push_back(content_type);

    std::string body = "<html><head><title>"       + std::to_string(code) + " " + reason(code) + 
                       "</title></head><body><h1>" + std::to_string(code) + " " + reason(code) +
                       "</h1></body></html>";

    respond(client_socket, code, headers, (uint8_t *) body.c_str(), body.length());
}

std::string Server::reason(uint16_t code) {
    // Mensagens de erro de acordo com cada códgo
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

    // Retorna o MIME type de acordo com a extensão do arquivo.
    if (extension == "html") return "text/html; charset=\"utf-8\""; 
    if (extension == "css")  return "text/css";      
    if (extension == "js")   return "text/javascript"; 
    if (extension == "php")  return "text/x-php";      
    if (extension == "gif")  return "image/gif";       
    if (extension == "ico")  return "image/x-icon";    
    if (extension == "jpg")  return "image/jpeg";      
    if (extension == "png")  return "image/png";       

    return "";
}

bool Server::load(FILE* file, uint8_t** content, size_t* length) {
    char* cont = (char*) malloc(sizeof(char));
    int size = 1;
    int temp = size;
    while(!feof(file)) {
        fread(&cont[size-1], 1, 1, file);
        if (feof(file)) cont[size-1] = '\0';
        size++;
        if (size > temp && !feof(file)) {
            cont = (char*) realloc(cont, sizeof(char) * size);
            temp = size;
        }
    };
    *length = size-2;
    *content = (uint8_t*) cont;
    return true;

}