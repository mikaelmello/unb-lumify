#include "../include/sockets.hpp"

#include <iostream>

using namespace Socket;

BaseSocket::BaseSocket(int type, int flags) {

    // Inicializa socketInfo
    socketInfo = new addrinfo;
    std::memset(socketInfo, 0, sizeof(addrinfo));

    // Prenche a estrutura de endereço do socket, sempre IPv4
    socketInfo->ai_family = AF_INET;
    socketInfo->ai_socktype = type;
    socketInfo->ai_flags = flags;

    // Mesma família (IPv4) e tipo de socket da socketInfo
    socketFd = socket(AF_INET, type, 0);
    if (socketFd == -1) {
        throw SocketException("Could not create socket. Error: " + std::string(strerror(errno)));
    }

    // Permite reuso de endereço (evita "Address already in use")
    int optval = 1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

BaseSocket::BaseSocket(int socket, int type, addrinfo sInfo, unsigned int port, bool isBound) :
    socketFd(socket), portUsed(port), isBound(isBound) {

    // Verifica que o endereço é IPv4 e do mesmo tipo definido pelo parâmetro
    if (sInfo.ai_family != AF_INET || sInfo.ai_socktype != type) {
        throw SocketException("sInfo passed is not defined as the defined type and protocol IPv4");
    }

    // Copia as inforamações do sInfo (sockaddr).
    socketInfo = new addrinfo(sInfo);

    // Permite reuso de endereço (evita "Address already in use")
    int optval = 1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

BaseSocket::~BaseSocket() {
    // Libera memória e encerra o socket
    if (socketInfo) freeaddrinfo(socketInfo);
    close();
}

void BaseSocket::bind(unsigned int port) {

    // Não é possivel vincular um socket que já está vinculado
    // Logo se a porta desejada for a mesma que já está vinculada, nada acontece.
    // Uma exceção é lançada caso contrário
    if (isBound && portUsed != port) {
        throw ConnectionException("Socket is already bound to port " 
            + std::to_string(portUsed));
    }
    else if (isBound && portUsed == port) {
        return;
    }

    // Pela documentação:
    //     If the AI_PASSIVE bit is set it indicates that the
    // returned socket address structure is intended for use
    // in a call to bind(2). In this case, if the hostname 
    // argument is the null pointer, then the IP address portion
    // of the socket address structure will be set to INADDR_ANY 
    // for an IPv4 address
    socketInfo->ai_flags |= AI_PASSIVE;
    getAddrInfo("NULL", port);


    int result;
    int totalIPv4Addrs = 0; // Conta quandos endereços IPv4 foram encontrados
    addrinfo * newAddrInfo;

    for (newAddrInfo = socketInfo; newAddrInfo; newAddrInfo = newAddrInfo->ai_next) {

        // Ignora qualquer endereço não IPv4
        if (newAddrInfo->ai_family == AF_INET) totalIPv4Addrs++;
        else continue;

        // Tenta vincular a qualquer endereço IPv4 encontrado.
        result = ::bind(socketFd, newAddrInfo->ai_addr, newAddrInfo->ai_addrlen);
        if (result == -1) continue;

        // Se a operação é bem-sucedida, seta as variáveis e retorna
        portUsed = port;
        isBound = true;
        return;
    }

    // Se nenhum endereço IPv4 é lançado, lança exceção
    if (totalIPv4Addrs == 0) {
        throw ConnectionException("Could not find IPv4 addresses to bind on port " 
            + std::to_string(port) + ".");
    }

    // Lança exceção caso não seja possível vincular a nenhum IPv4 encontrado.
    throw ConnectionException("Could not bind to port " + std::to_string(port) 
            + ". Error: " + std::string(strerror(errno)));
}

void BaseSocket::close() {

    // Se a socket não foi setada ou já foi fechada, nada acontece.
    if (socketFd == -1 || isClosed) return;

    int result = ::close(socketFd);
    if (result == -1) {
        throw SocketException("Could not close socket. Error: " + 
            std::string(strerror(errno)));
    }

    isBound = false;
    isClosed = true;
    portUsed = -1;
    socketFd = -1;

}

void BaseSocket::getAddrInfo(const std::string address, unsigned int port) {

    addrinfo   hints = *socketInfo;
    addrinfo * newSocketInfo;
    const char * cAddress;

    // cAddress = NULL significa que o getaddrinf() buscará por endereços
    // locais da maquina
    if (address == "NULL") cAddress = NULL;
    else cAddress = address.c_str();

    int result = getaddrinfo(cAddress, std::to_string(port).c_str(), &hints, &newSocketInfo);

    if (result != 0) {
        throw ConnectionException("Error on getaddrinfo(): " + 
            std::string(gai_strerror(result)));
    }

    // Libera a memoria da socketInfo atual para ela ser realocada por getaddrinfo()
    if (socketInfo) freeaddrinfo(socketInfo);
    socketInfo = newSocketInfo;  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                  TCPSOCKET
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

TCPSocket::TCPSocket(int flags) : BaseSocket(SOCK_STREAM, flags) {}

TCPSocket::TCPSocket(int socket, addrinfo socketInfo, unsigned int portUsed, bool isBound, bool isListening, bool isConnected) : 
    BaseSocket(socket, SOCK_STREAM, socketInfo, portUsed, isBound), isListening(isListening), isConnected(isConnected) {
}

void TCPSocket::connect(const std::string& address, unsigned int port) {

    if (isListening) {
        throw SocketException("Socket is already listening incoming connections on port " 
                                + std::to_string(portUsed));
    }
    if (isConnected) {
        throw SocketException("Socket is already connected to an address");
    }
    
    getAddrInfo(address, port);

    int result;
    int totalIPv4Addrs = 0;
    addrinfo * newAddrInfo;

    for (newAddrInfo = socketInfo; newAddrInfo; newAddrInfo = newAddrInfo->ai_next) {

        if (newAddrInfo->ai_family == AF_INET) totalIPv4Addrs++;
        else continue;

        result = ::connect(socketFd, newAddrInfo->ai_addr, newAddrInfo->ai_addrlen);
        if (result == -1) continue;

        isConnected = true;
        return;
    }

    if (totalIPv4Addrs == 0) {
        throw ConnectionException("Could not find IPv4 addresses to connect to " + address + 
                                  " on port " + std::to_string(port) + ".");
    }

    throw ConnectionException("Could not connect to address " + address +
                              " on port " + std::to_string(port) + 
                              ". Error: " + std::string(strerror(errno)));

}
        
void TCPSocket::listen(unsigned int backlog) {

    if (!isBound) {
        throw SocketException("Can not listen when socket is not bound to any port");
    }
    if (isListening) {
        throw SocketException("Socket is already listening incoming connections on port " 
                                + std::to_string(portUsed));
    }

    int result = ::listen(socketFd, backlog);
    if (result == -1) {
        throw ConnectionException("Could not listen on port " + std::to_string(portUsed) 
                                + ". Error: " + std::string(strerror(errno)));
    }

    isListening = true;

}

TCPSocket TCPSocket::accept() {

    if (!isListening) {
        throw SocketException("Socket is not listening to incoming connections to accept one");
    }

    sockaddr clientAddress;
    socklen_t sin_size = sizeof(sockaddr);
    int clientFd = ::accept(socketFd, &clientAddress, &sin_size);

    if (clientFd == -1) {
        throw ConnectionException("Error while accepting a connection. Error: " 
            + std::string(strerror(errno)));
    }

    addrinfo clientInfo;
    memset(&clientInfo, 0, sizeof(addrinfo));

    clientInfo.ai_family = AF_INET;
    clientInfo.ai_socktype = SOCK_STREAM;
    clientInfo.ai_addr = &clientAddress;

    return TCPSocket(clientFd, clientInfo, portUsed, false, false, true);

}
        
void TCPSocket::send(const std::string& message, int flags) {

    if (isListening || !isConnected) {
        throw SocketException("Can't send message from a socket that is not connected");
    }

    int msgLength = message.length();
    int bytesLeft = msgLength;
    int bytesSent = 0;
    const char* cMessage = message.c_str();

    while (bytesSent < msgLength) {

        const char* cMessageLeft = cMessage + bytesSent;

        int result = ::send(socketFd, cMessageLeft, bytesLeft, flags | MSG_NOSIGNAL);
        if (result == -1) {
            throw ConnectionException("Could not send message. Error: " 
                + std::string(strerror(errno)));
        }

        bytesLeft -= result;
        bytesSent += result;

    }

}

std::string TCPSocket::recv(unsigned int maxlen, int flags) {

    if (isListening || !isConnected) {
        throw SocketException("Can't receive message in a socket that is not connected");
    }

    char buffer[maxlen+1];
    int result = ::recv(socketFd, buffer, maxlen, flags);

    if (result == -1) {
        throw ConnectionException("Could not receive message. Error: " 
            + std::string(strerror(errno)));
    }
    if (result ==  0) {
        isConnected = false;
        throw ClosedConnection("Client closed connection.");
    }
    buffer[result] = '\0';
    std::string message(buffer);

    return message;

}

void TCPSocket::close() {

    BaseSocket::close();

    isConnected = false;
    isListening = false;


}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                  UDPRECV
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

UDPRecv::UDPRecv(std::string address, std::string msg, unsigned int port) :
    address(address), msg(msg), port(port) {
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                  UDPSOCKET
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UDPSocket::sendto(const std::string& address, unsigned int port, 
    const std::string& message, int flags) {

}

UDPRecv UDPSocket::recvfrom(const std::string& address, unsigned int port,
    unsigned int maxlen, int flags) {

}

