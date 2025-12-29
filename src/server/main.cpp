#include "Server.h"

#include <iostream>
#include <atomic>
#include <csignal>

std::atomic running(true);

void handleSignals()
{
    running.store(false);
}

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    signal(SIGINT, reinterpret_cast<__sighandler_t>(handleSignals));
    signal(SIGTERM, reinterpret_cast<__sighandler_t>(handleSignals));
    signal(SIGHUP, reinterpret_cast<__sighandler_t>(handleSignals));
    signal(SIGQUIT, reinterpret_cast<__sighandler_t>(handleSignals));

    auto *server = new Server(std::stoi(argv[1]), running);
    server->run();
    delete server;
    return EXIT_SUCCESS;
}
