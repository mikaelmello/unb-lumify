#include <bits/stdc++.h>
#include "sockets.hpp"

using namespace Socket;
using namespace std;

int main() {
    TCPSocket sv1(0);

    try {
        sv1.bind(1234);
        sv1.listen();
        std::shared_ptr<TCPSocket> client = sv1.accept();
        printf("Cliente conectado\n");
        try {
            for (;;) {
                int input;
                string legal;
                cin >> input >> legal;
                if (input == 1) std::cout << client->recv(256) << std::endl;
                else {
                    client->send(legal + "\n");
                }
            }
        }
        catch (const ClosedConnection & x) {
            printf("Connection closed\n");
        }

    }
    catch( std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}