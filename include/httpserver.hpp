#ifndef HTTPSERVER_H
#define HTTPSERVER_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <utility>
#include <atomic>
#include "spdlog/spdlog.h"

#include "sockets.hpp"

namespace HTTP {

/// Struct que é lançada como exceção caso algum request
/// precise ser respondido com um código de erro.
struct RequestError {

    /// Construtor
    /// @param code Código do erro.
    RequestError(uint16_t code) : code(code) {}

    /// Código do erro.
    uint16_t code;
};

/// Estrutura de uma solicitação recebida.
struct Request {

    /// Método da solicitação.
    std::string method; 

    /// Caminho relativo do arquivo solicitado.
    std::string file_path;

    /// Caminho absoluto do arquivo solicitado.
    std::string absolute_path;

    /// Versão HTTP definida na solicitação.
    std::string http_version;

    /// URI da solicitação, já dividida em strings.
    std::vector<std::string> uri;

    /// Cabeçalhos da solicitação. Cada entrada é um par chave-valor
    /// de strings.
    std::vector<std::pair<std::string, std::string>> request_headers;

    /// Parâmetros presentes na URI.
    std::vector<std::pair<std::string, std::string>> uri_parameters;
};

/// Servidor HTTP.
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

        /// Aceita conexões ao servidor por tempo indefinido.
        void accept();

        /// Define o que fazer com um request.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        void handle_request(std::shared_ptr<Socket::TCPSocket> client_socket);

        /// Define o que responder ao cliente conectado de acordo com seu request.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        /// @param client_request Struct com as informações da solicitação.
        void handle_response(std::shared_ptr<Socket::TCPSocket> client_socket, 
                             HTTP::Request client_request);

        /// Transfere o arquivo requisitado para o cliente.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        /// @param client_request Struct com as informações da solicitação.
        /// @param file_type Tipo de arquivo requisitado.
        void transfer(std::shared_ptr<Socket::TCPSocket> client_socket, 
                      HTTP::Request client_request, const std::string& file_type);

        /// Interpreta um arquivo PHP antes de enviar algo ao cliente.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        /// @param client_request Struct com as informações da solicitação.
        void interpret(std::shared_ptr<Socket::TCPSocket> client_socket, 
                       HTTP::Request client_request);

        /// Método que envia de fato a resposta ao cliente por meio do socket.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        /// @param code Código da resposta.
        /// @param headers Cabeçalhos da resposta.
        /// @param body Ponteiro para os bytes do corpo da resposta.
        /// @param length Tamanho do corpo da resposta.
        void respond(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code, 
                     const std::vector<std::pair<std::string, std::string>>& headers, 
                     uint8_t * body, uint32_t length);

        /// Método que envia uma resposta que indica um erro.
        /// @param client_socket TCPSocket do cliente conectado ao servidor.
        /// @param code Código do erro
        void error(std::shared_ptr<Socket::TCPSocket> client_socket, uint16_t code);

        /// Descrição um pouco mais detalhada de um erro de acordo com seu código.
        /// @param code Código do erro.
        /// @return String contendo a descrição.
        std::string reason(uint16_t code);

        /// Decodifica uma string no padrão URL-encoded.
        /// @param url String com a URL
        /// @return String com a URL decodificada.
        std::string url_decode(const std::string& url);

        /// Traduz a extensão de arquivo em uma string contendo o tipo MIME dele.
        /// @param extension Extensão do arquivo.
        /// @return String contendo o MIME type.
        std::string lookup_type(const std::string& extension);

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
        uint32_t port;;

};

} // end namespace HTTP

#endif