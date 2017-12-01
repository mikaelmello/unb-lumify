#ifndef P2PSERVER_H
#define P2PSERVER_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "spdlog/spdlog.h"
#include <set>
#include "helpers.hpp"
#include "sockets.hpp"
#include "protocol.hpp"

namespace P2P {

struct Peer {
    Peer(uint16_t id, std::string name, std::string host) :
        id(id), name(name), host(host) {}
    Peer() : id(-1) {}
    uint16_t id;
    std::string name;
    std::string host;
};

/// Servidor P2P.
class Server {

    public:

        /// Construtor
        Server(uint16_t max_peers = 255);

        /// Inicia o servidor
        /// @param path Caminho raiz do servidor
        /// @param port Porta usada pelo servidor.
        void start(const std::string& path, uint32_t port = 44777);

        /// Interrompe a execução do servidor.
        void stop();

    private:

        /// Aceita conexões ao servidor por tempo indefinido.
        void accept();

        /// Define o que fazer com um request.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        void handle_request(std::shared_ptr<Socket::TCPSocket> client_socket);

        /// Consegue os IDs de todos os peer conectados na rede
        /// @return Um conjunto com todos os IDs na rede.
        std::set<uint16_t>  get_peer_ids();

        /// Verifica se um no esta vivo na rede.
        /// @param peer_id ID do Peer na rede
        /// @return Booleano indicando se o peer esta conectado na rede
        bool check_peer(uint16_t peer_id);

        void add_peer(Peer new_peer);

        /// Quantidade de nos conhecidos na rede
        /// @return Quantidade de nos conhecidos na rede
        uint16_t get_peers_qty();

        /// Verifica se o maximo de nos foi atingido
        /// @return Booleando indicando se o numero maximo de nos foi atingido
        bool max_peers_reached();

        /// Procura por novos nos na rede e verifica nos que morreram
        void check_live_peers();

        void recv_discovers();

        /// Booleano que sinaliza o fim da execução do servidor.
        std::atomic<bool> is_finished;

        /// Contabiliza o número de threads ativos criados por esta classe.
        std::atomic<int> threads_qty;

        std::atomic<int> current_id;

        /// Logger
        std::shared_ptr<spdlog::logger> log;

        /// Socket do servidor, onde há a espera por conexões.
        Socket::TCPSocket server_socket;

        /// Caminho raiz do servidor.
        std::string root_path;

        std::string my_name;

        /// Porta usada pelo servidor.
        uint32_t port;

        /// Peer ID do proprio node
        uint16_t my_id;

        /// Numero limite de nos da rede P2P
        uint16_t max_peers;

        std::map<uint16_t, Peer> known_peers;
        std::map<uint16_t, Peer> known_peers_backup;
        std::map<std::string, uint16_t> nickname_to_peer_id;
        std::mutex barrier_known_peers;

        const uint16_t UDP_DISCOVER = 44599;
        const uint16_t UDP_FOUND    = 44600;

};

} // end namespace P2P

#endif