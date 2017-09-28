#include <bits/stdc++.h>
#include "sockets.hpp"

using namespace Socket;
using namespace std;

int main() {
    UDPSocket sv1(0);

    try {
        sv1.bind(1234);
        string add;
        int port;
        for (;;) {
            int input;
            string legal;
            cin >> input >> legal;
            if (input == 1) {
                
                UDPRecv xd = sv1.recvfrom(256);
                cout << xd.getMsg() << endl;
                add = xd.getAddress();
                port = xd.getPort();

            }
            else {
                sv1.sendto(add, port, legal + "\n");
            }
        }
    }
    catch( std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}