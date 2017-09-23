#include <bits/stdc++.h>
#include <sockets.hpp>

using namespace Socket;

int main() {
    TCPSocket sv1(0);

    try {
        sv1.bind(1170);
        sv1.listen();
        TCPSocket client = sv1.accept();
        client.send("Welcome !\n");
        client.close();
    }
    catch( std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}