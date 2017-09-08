#include "../include/sockets.hpp"

using namespace Socket;

TCPSocket::TCPSocket(int flags) {
    socketInfo = new addrinfo;

    std::memset(socketInfo, 0, sizeof(addrinfo));

    socketInfo->ai_family = AF_INET;
    socketInfo->ai_socktype = SOCK_STREAM;
    socketInfo->ai_flags = flags;

    // Same family and socktype as socketInfo
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        throw "Could not create socket";
    }

    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

TCPSocket::TCPSocket(int socket, addrinfo sInfo, bool isBound, bool isListening, bool isConnected) :
                         socketFd(socket), isBound(isBound), isListening(isListening), isConnected(isConnected) {


    if (sInfo.ai_family != AF_INET || sInfo.ai_socktype != SOCK_STREAM) throw "Not TCP/IPv4 addrinfo";

    socketInfo = new addrinfo(sInfo);
    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

TCPSocket::~TCPSocket() {
    freeaddrinfo(socketInfo);
    if (isConnected) close();
}

void TCPSocket::bind(unsigned int port) {

    if (isBound) throw "Already bound";

    // Per documentation: 
    //     If the AI_PASSIVE bit is set it indicates that the
    // returned socket address structure is intended for use
    // in a call to bind(2). In this case, if the hostname 
    // argument is the null pointer, then the IP address portion
    // of the socket address structure will be set to INADDR_ANY 
    // for an IPv4 address
    socketInfo->ai_flags |= AI_PASSIVE;
    getAddrInfo("NULL", port);

    int result;
    addrinfo * newAddrInfo;

    for (newAddrInfo = socketInfo; newAddrInfo; newAddrInfo = newAddrInfo->ai_next) {

        result = ::bind(socketFd, newAddrInfo->ai_addr, newAddrInfo->ai_addrlen);
        if (result == -1) continue;

        isBound = true;
        return;
    }

    throw "Could not bind";
}

void TCPSocket::connect(const std::string& address, unsigned int port) {

    if (isListening) throw "Can't connect when already bound and listening";
    if (isConnected) throw "Already connected";
    
    getAddrInfo(address, port);

    int result;
    addrinfo * newAddrInfo;

    for (newAddrInfo = socketInfo; newAddrInfo; newAddrInfo = newAddrInfo->ai_next) {

        result = ::connect(socketFd, newAddrInfo->ai_addr, newAddrInfo->ai_addrlen);
        if (result == -1) continue;

        isConnected = true;
        return;
    }

    throw "Could not connect";

}
        
void TCPSocket::listen(unsigned int backlog) {

    if (!isBound) throw "Not bound to any port to listen";
    if (isListening) throw "Already listening";

    int result = ::listen(socketFd, backlog);
    if (result == -1) throw "Could not listen";

    isListening = true;

}

TCPSocket TCPSocket::accept() {

    if (!isListening) throw "Can't accept while not listening";

    sockaddr_in * clientAddress = new sockaddr_in;
    memset(&clientAddress, 0, sizeof(sockaddr_in));
    socklen_t sin_size = sizeof(sockaddr_in);
    int clientFd = ::accept(socketFd, (sockaddr *)&clientAddress, &sin_size);

    if (clientFd == -1) throw "No connections to accept";

    addrinfo clientInfo;
    memset(&clientInfo, 0, sizeof(addrinfo));

    clientInfo.ai_family = AF_INET;
    clientInfo.ai_socktype = SOCK_STREAM;
    clientInfo.ai_addr = (sockaddr*)clientAddress;

    return TCPSocket(clientFd, clientInfo, false, false, true);

}
        
void TCPSocket::send(const std::string& message, int flags) {

    if (isListening || !isConnected) throw "Can't send from this socket";

    int msgLength = message.length();
    int bytesLeft = msgLength;
    int bytesSent;
    const char* cMessage = message.c_str();

    while (bytesSent < msgLength) {

        const char* cMessageLeft = cMessage + bytesSent;

        int result = ::send(socketFd, cMessageLeft, bytesLeft, flags);
        if (result == -1) throw "Error while sending";

        bytesLeft -= result;
        bytesSent += result;

    }

}

std::string TCPSocket::recv(unsigned int maxlen, int flags) {

    if (isListening || !isConnected) throw "Can't receive from this socket";

    char buffer[maxlen];
    int result = ::recv(socketFd, buffer, maxlen, flags);

    if (result == -1) throw "Error on receive";
    if (result ==  0) isConnected = false;

    std::string message(buffer);

    return message;

}

void TCPSocket::close() {

    int result = ::close(socketFd);
    if (result == -1) throw "Error on closing socketFd";

    isConnected = false;

}

void TCPSocket::getAddrInfo(const std::string address, unsigned int port) {

    addrinfo hints = *socketInfo;
    const char * cAddress;

    if (address == "NULL") cAddress = NULL;
    else cAddress = address.c_str();

    int result = getaddrinfo(cAddress, std::to_string(port).c_str(), &hints, &socketInfo);


    if (result != 0) {
        throw "Error on getaddrinfo(): " + std::string(gai_strerror(result));
    }
}