#include <bits/stdc++.h>
#include "sockets.hpp"
#include <chrono>
#include <thread>

using namespace Socket;
using namespace std;

int main() {
    UDPSocket cl1;

    try {
        string add;
        int port;
        for (;;) {
            int input;
            string legal;
            cin >> input >> legal;
            if (input == 1) {
                
                UDPRecv xd = cl1.recvfrom(256);
                cout << xd.get_msg() << endl;
                add = xd.get_address();
                port = xd.get_port();

            }
            else {
                cl1.sendto("255.255.255.255", 1234, legal + "\n");
            }

        }
    }
    catch( std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}