#include <bits/stdc++.h>
#include "../include/sockets.hpp"
#include <chrono>
#include <thread>

using namespace Socket;
using namespace std;

int main() {
    TCPSocket cl1(0);

    try {
        cl1.connect("localhost", 1234);
        for (;;) {
            int input;
            string legal;
            cin >> input >> legal;
            if (input == 1) std::cout << cl1.recv(256) << std::endl;
            else {
                cl1.send(legal + "\n");
            }

        }
    }
    catch( std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}