#ifndef SOCKETS_H
#define SOCKETS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdexcept>
#include <cstring>
#include <string>

namespace Socket {
 
class ConnectionException : public std::runtime_error {
    public:
        ConnectionException(const std::string& error) : std::runtime_error(error) {}
};

class ClosedConnection : public ConnectionException {
    public:
        ClosedConnection(const std::string& error) : ConnectionException(error) {}
};

/** Classe base de um wrapper de um socket padrão IPv4.
 */
class BaseSocket {
    public:

        /// Vincula o socket a alguma porta.
        /// @param port Número da porta que o socket será vinculado.
        void bind(unsigned int port);
        
        /// Encerra o socket.
        void close();

    protected:

        /// Cria um socket IPv4 de acordo com o protocolo selecionado.
        /// @param type Tipo do socket a ser criado, SOCK_STREAM ou SOCK_DGRAM.
        /// @param flags Flags opcionais para a chamada de getaddrinfo()
        BaseSocket(int type, int flags = 0);

        /// Cria um socket IPv4 copiando as variaveis definidas nos parametros.
        /// @param socket       Descritor de arquivo da socket já criada.
        /// @param type         Tipo do socket (SOCK_DGRAM ou SOCK_STREAM).
        /// @param socketInfo   Addrinfo TCP/IPv4 que já contêm endereços preenchidos.
        /// @param portUsed     Porta na qual o socket foi vinculado.
        /// @param isBound      Indica se o socket está vinculado a alguma porta.
        BaseSocket(int socket, int type, addrinfo socketInfo, int portUsed, bool isBound);

        /// Destrutor. Libera memórias alocadas e encerra o socket.
        ~BaseSocket();

        /// Preenche socketInfo com endereços possíveis de se realizar chamadas
        /// de bind() ou connect().
        /// @param address Endereço a ser procurado (IPv4 ou domínio).
        /// @param port Porta a ser realizada a busca.
        void getAddrInfo(const std::string address, unsigned int port);

        /// Contém as informações do socket.
        /// Socket será sempre TCP/IPv4 ou UDP/IPv4.
        addrinfo * socketInfo;
        
        /// Socket file descriptor.
        int socketFd = -1;

        /// Port used by bind()
        int portUsed = -1;

        /// Is socket bound to a specific port
        bool isBound = false;

        /// Is socket already closed
        bool isClosed = false;

};


/** Wrapper de um socket TCP/IPv4.
 *  
 *      Para usar como um servidor (recebendo conexões) construa o objeto,
 *  faça o bind() e o listen() e aceite diferentes conexões com diferentes
 *  chamadas à accept(). Para trocar informações com um cliente conectado
 *  utilize as funções send() e recv() do objeto retornado por accept().
 *      Para usar como um cliente basta construir o objeto e usar a função
 *  connect() para se conectar a um servidor, utilizando as funções send()
 *  e recv() para trocar informações com o servidor.
 *      A conexão é fechada normalmente usando a função close().
 */
class TCPSocket : public BaseSocket {
    public:

        /// Cria um socket TCP/IPv4.
        /// @param flags Flags opcionais para a chamada de getaddrinfo()
        TCPSocket(int flags = 0);

        /// Cria um socket TCP/IPv4 copiando as variaveis definidas nos parametros.
        /// @param socket       Descritor de arquivo da socket já criada.
        /// @param socketInfo   Addrinfo TCP/IPv4 que já contêm endereços preenchidos.
        /// @param portUsed     Porta na qual o socket foi vinculado.
        /// @param isBound      Indica se o socket está vinculado a alguma porta.
        /// @param isListening  Indica se o socket já está ouvindo em alguma porta.
        /// @param isConnected  Indica se o socket está conectado a algum servidor.
        TCPSocket(int socket, addrinfo socketInfo, int portUsed, bool isBound, bool isListening, bool isConnected);
        
        /// Conecta a socket a algum endereço.
        /// @param address std::string contendo o endereço IP ou domínio a ser conectado.
        /// @param port    Porta a qual se conectar.
        void connect(const std::string& address, unsigned int port);
        
        /// Recebe conexões na porta definida previamente pela função bind().
        /// @param backlog Número máximo de conexões pendentes.
        void listen(unsigned int backlog = 5);
        
        /// Aceita uma conexão pendente após chamar a função listen().
        /// @return Objeto TCPSocket conectado ao cliente aceito.
        TCPSocket accept(); 
        
        /// Envia uma string ao endereço conectado.
        /// @param message Mensagem a ser enviada.
        /// @param flags   Flags opcionais para o envio da mensagem ao socket.
        void send(const std::string& message, int flags = 0);
        
        /// Recebe uma string do endereço conectado.
        /// @param maxlen Tamanho máximo da mensagem a ser recebida.
        /// @param flags  Flags opcionais para o recebimento da mensagem.
        /// @return std::string contendo a mensagem recebida.
        std::string recv(unsigned int maxlen, int flags = 0);
        
        /// Encerra o socket.
        void close();

    private:
        
        /// Is socket connected to another socket
        bool isConnected = false;

        /// Is socket listening for incoming connections
        bool isListening = false;

};


/** Wrapper de um socket UDP/IPv4.
 *  
 *      Para usar como um servidor (recebendo conexões) construa o objeto,
 *  faça o bind() receba mensagens usando recv() e as responda com send().
 *      Para usar como um cliente basta construir o objeto e usar a função
 *  send() e recv() para trocar informações com o servidor.
 *      A conexão é fechada normalmente usando a função close().
 */
class UDPSocket {


};



} // End namespace Socket

#endif