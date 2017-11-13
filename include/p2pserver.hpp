#ifndef P2PSERVER_H
#define P2PSERVER_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "spdlog/spdlog.h"

#include "sockets.hpp"

namespace P2P {

/// Servidor P2P.
class Server {

    public:

        /// Construtor
        Server();

        /// Inicia o servidor
        /// @param path Caminho raiz do servidor
        /// @param port Porta usada pelo servidor.
        void start(const std::string& path, uint32_t port = 8080);

        /// Interrompe a execução do servidor.
        void stop();

    private:

        

        /// Booleano que sinaliza o fim da execução do servidor.
        std::atomic<bool> is_finished;

        /// Contabiliza o número de threads ativos criados por esta classe.
        std::atomic<int> threads_qty;

        /// Logger
        std::shared_ptr<spdlog::logger> log;

        /// Socket do servidor, onde há a espera por conexões.
        Socket::TCPSocket server_socket;

        /// Caminho raiz do servidor.
        std::string root_path;

        /// Porta usada pelo servidor.
        uint32_t port;

};

} // end namespace P2P

#endif