#include <iostream>
#include <signal.h>
#include "httpserver.hpp"

void handler(int param);

bool signaled = false;

int main() {

    signal(SIGINT, handler);
    
    //P2PServer sv0;
    //sv0.start();

    HTTP::Server sv1;
    sv1.start("www");

    while (!signaled);

    //sv1.stop();
    
}

void handler(int signal) {
    if (signal == SIGINT) {
        signaled = true;
    }
}