#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>

namespace Socket {

class TCPSocket {
    public:
        
        TCPSocket(int flags);

        TCPSocket(int socket, addrinfo socketInfo, bool isListening, bool isConnected, bool isBound);

        ~TCPSocket();

        void bind(unsigned int port);
        
        void connect(const std::string& address, unsigned int port);
        
        void listen(unsigned int backlog);
        
        TCPSocket accept(); // Change return type
        
        void send(const std::string& message, int flags);
        
        std::string recv(unsigned int maxlen, int flags);
        
        void close();

    private:

        void getAddrInfo(const std::string address, unsigned int port);

        addrinfo * socketInfo;
        
        int  socketFd = -1;
        
        bool isBound = false;
        
        bool isConnected = false;

        bool isListening = false;

        bool isClosed = false;

};

class UDPSocket {


};



} // End namespace Socket

#endif