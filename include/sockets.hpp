#ifndef SOCKETS_H
#define SOCKETS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>

namespace Socket {

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
class TCPSocket {
    public:

        /// Cria um socket TCP/IPv4.
        /// @param flags Flags opcionais para a chamada de getaddrinfo()
        TCPSocket(int flags = 0);

        /// Cria um socket TCP/IPv4 copiando as variaveis definidas nos parametros.
        /// @param socket       Descritor de arquivo da socket já criada.
        /// @param socketInfo   Addrinfo TCP/IPv4 que já contêm endereços preenchidos.
        /// @param isBound      Indica se o socket está vinculado a alguma porta.
        /// @param isListening  Indica se o socket já está ouvindo em alguma porta.
        /// @param isConnected  Indica se o socket está conectado a algum servidor.
        TCPSocket(int socket, addrinfo socketInfo, bool isBound, bool isListening, bool isConnected);

        /// Destrutor. Libera memórias alocadas e encerra o socket.
        ~TCPSocket();

        /// Vincula o socket a alguma porta.
        /// @param port Número da porta que o socket será vinculado.
        void bind(unsigned int port);
        
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

        /// Preenche socketInfo com endereços possíveis de se realizar chamadas
        /// de bind() ou connect().
        /// @param address Endereço a ser procurado (IPv4 ou domínio).
        /// @param port Porta a ser realizada a busca.
        void getAddrInfo(const std::string address, unsigned int port);

        /// Contém as informações do socket.
        /// Socket será sempre TCP/IPv4.
        addrinfo * socketInfo;
        
        /// Socket file descriptor.
        int  socketFd = -1;

        bool isBound = false;
        
        bool isConnected = false;

        bool isListening = false;

};

class UDPSocket {


};



} // End namespace Socket

#endif