#include <bits/stdc++.h>
#include "sockets.hpp"

using namespace Socket;
using namespace std;

int main() {
    UDPSocket sv1(0);

    try {
        sv1.bind(44599);
        string add;
        int port;
        for (;;) {
            int input;
            string legal;
            cin >> input >> legal;
            if (input == 1) {
                
                UDPRecv xd = sv1.recvfrom(256);
                cout << xd.get_msg() << endl;
                add = xd.get_address();
                port = xd.get_port();

            }
            else {
                sv1.sendto("192.168.0.10", 44600, legal);
            }
        }
    }
    catch( std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}