#include "p2pserver.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace P2P;

Server::Server(uint16_t max_peers) : max_peers(max_peers), is_finished(false),
    threads_qty(0), my_name("None") {
    try {
        // inicializa o logger
        log = spdlog::stdout_color_mt("p2pserver");
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

    this->current_id = 1;


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

        // Verifica por peers na rede
        std::thread t2([this] { this->check_live_peers(); });
        t2.detach();
        this->threads_qty++;

        // Responde por verificacoes de outros peers
        std::thread t3([this] { this->recv_discovers(); });
        t3.detach();
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
    log->warn("Initiating termination of P2P Server");
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

        // Recebe dados até que seja encontrado CRLFCRLF (fim de um request).
        while (request_str.find("[end]") == std::string::npos) {
            request_str += client_socket->recv(4096);
        }

        std::vector<std::string> tokens = Helpers::split(request_str, ":");
        if (tokens[0] == "PHP") handle_php(client_socket, tokens);
        else handle_fs(client_socket, tokens);
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

void Server::handle_fs(std::shared_ptr<Socket::TCPSocket> client_socket, std::vector<std::string> tokens) {
    if      (tokens[1] == "CREATE_FOLDER") ;// file_system.create_folder();
    else if (tokens[1] == "DELETE_FOLDER") ;// file_system.delete_folder();
    else if (tokens[1] == "UPDATE_FOLDER") ;// file_system.update_folder();

    else if (tokens[1] == "CREATE_FILE") ;// file_system.create_file();
    else if (tokens[1] == "DELETE_FILE") ;// file_system.delete_file();
    else if (tokens[1] == "UPDATE_FILE") ;// file_system.update_file();
}

void Server::handle_php(std::shared_ptr<Socket::TCPSocket> client_socket, std::vector<std::string> tokens) {
    if (tokens[1] == "NEW_NICK") {
        barrier_my_name.lock();
        uint16_t peer_id = nickname_to_peer_id[tokens[1]];
        std::string answer;
        if (peer_id != 0) error_php(client_socket, "Nome ja usado");
        else {
            client_socket->send("{\"error\":\"false\"}");
            this->my_name = tokens[2];
        }
        barrier_my_name.unlock();
    }
    else if (tokens[1] == "UPDATE") {
        std::string msg = "{\"error\":\"false\"";
        msg += ", \"users_qty\":\"" + std::to_string(get_peers_qty()) + "\"";
        /*
        msg += ", \"total_folders_qty\":\"" + file_system.get_folders_no() + "\"";
        msg += ", \"total_files_qty\":\"" + file_system.get_files_no() + "\"";
        msg += ", \"total_size\":\"" + file_system.get_total_size() + "\"";
        msg += ", \"cur_folders_qty\":\"" + file_system.get_current_path()->get_folders_no() + "\"";
        msg += ", \"cur_files_qty\":\"" + file_system.get_current_path()->get_files_no() + "\"";
        msg += ", \"cur_total_size\":\"" + file_system.get_current_path()->get_total_size() + "\"";
        */
        msg += "}";
        client_socket->send(msg);
    }
    else error_php(client_socket, "Comando nao existente");
}

void Server::error_php(std::shared_ptr<Socket::TCPSocket> client_socket, const std::string& reason) {
    std::string error_msg = "{\"error\":\"true\",\"reason\":\"" + reason + "\"}";
    client_socket->send(error_msg);
}

void Server::check_live_peers() {
    Socket::UDPSocket discover;
    discover.set_timeout(5);
    discover.bind(UDP_FOUND);

    while(!this->is_finished) {

        auto start = std::chrono::system_clock::now();

        barrier_known_peers.lock();
        known_peers_backup = known_peers;
        known_peers.clear();
        barrier_known_peers.unlock();

        try {
            discover.sendto("255.255.255.255", UDP_DISCOVER, "DISCOVER");
        }
        catch (const Socket::ConnectionException& e) {
            log->error(e.what());
            log->error("Trying again in 30 seconds");
        }

        try {
            while(!this->is_finished) {
                Socket::UDPRecv response = discover.recvfrom(256);
                std::string message = response.get_msg();
                std::vector<std::string> tokens = Helpers::split(message, ':');

                uint16_t new_peer_id = nickname_to_peer_id[tokens[1]];

                if (tokens.size() != 2 || tokens[0] != "FOUND" || 
                    tokens[1].length() <= 0) {  
                    // if answer received is not the one we want
                    continue;
                }
                else if (new_peer_id != 0 && 
                    known_peers_backup[new_peer_id].host != response.get_address()) {
                    // if replier answered with a nickname that is not his
                    // send message indicating NICK error
                    discover.sendto(response.get_address(), UDP_DISCOVER, "FAIL:NICK");
                    continue;
                }

                if (new_peer_id == 0) {
                    new_peer_id = this->current_id;
                    this->current_id += 1;
                }

                Peer new_peer(new_peer_id, tokens[1], response.get_address());
                add_peer(new_peer);
                discover.sendto(response.get_address(), UDP_DISCOVER, "SUCCESS");
            }
        }
        catch (const Socket::ConnectionException& e) {
            // stop waiting for new answers
        }

        
        // pausar ate completar 30 segundos desde o ultimo broadcast
        // aumenta de 1 em 1 segundo para verificar sempre se a execucao foi
        // finalizada
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        while (!this->is_finished && elapsed_seconds.count() <= 30) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            end = std::chrono::system_clock::now();
            elapsed_seconds = end - start;
        }

    }

    this->threads_qty--;
}

void Server::recv_discovers() {
    Socket::UDPSocket discover;
    discover.set_timeout(3);
    discover.bind(UDP_DISCOVER);

    while(!this->is_finished) {

        barrier_my_name.lock();
        if (this->my_name == "None") {
            barrier_my_name.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        barrier_my_name.unlock();

        try {
            Socket::UDPRecv response = discover.recvfrom(256);
            if (response.get_msg() != "DISCOVER") continue;

            barrier_my_name.lock();
            discover.sendto(response.get_address(), UDP_FOUND, 
                "FOUND:" + this->my_name);
            barrier_my_name.unlock();

            response = discover.recvfrom(256);
            std::vector<std::string> tokens = Helpers::split(response.get_msg(), ':');
            if (tokens[0] == "FAIL") {
                if (tokens[1] == "NICK") {
                    // TODO: Enviar aviso ao usuário web dizendo que o nick escolhido nao serve
                }
            }

        }
        catch (const Socket::ConnectionException& e) {
        }
    }

    this->threads_qty--;

}

void Server::add_peer(Peer new_peer) {

    barrier_known_peers.lock();

    known_peers[new_peer.id] = new_peer;
    nickname_to_peer_id[new_peer.name] = new_peer.id;

    barrier_known_peers.unlock();
}

bool Server::check_peer(uint16_t peer_id) {
    // send specific packet
}

uint16_t Server::get_peers_qty() {

    uint16_t size;

    barrier_known_peers.lock();

    size = known_peers.size();
    uint16_t my_id = nickname_to_peer_id[this->my_name];
    if (my_id == 0 || known_peers[my_id].id == 0) size++;

    barrier_known_peers.unlock();

    return size;

}

bool Server::max_peers_reached() {
    return (get_peers_qty() > this->max_peers);
}