#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>
#include "httpserver.hpp"
#include "p2pserver.hpp"

void handler(int param);

bool signaled = false;

int main() {

    signal(SIGINT, handler);
    
    P2P::Server  sv0;
    HTTP::Server sv1;
    sv0.start("files");
    sv1.start("www");

    while (!signaled) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    sv0.stop();
    sv1.stop();
    
}

void handler(int signal) {
    if (signal == SIGINT) {
        signaled = true;
    }
}