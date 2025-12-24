#include <iostream>

#include "network/Server.h"

int main(const int argc, char *argv[])
{
	if(argc != 2){
		std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
		return EXIT_FAILURE;
	}
	auto *server = new Server(std::stoi(argv[1]));
	server->run();
	delete server;
    return EXIT_SUCCESS;
}
