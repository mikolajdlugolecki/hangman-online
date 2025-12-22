#include "Server.h"

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <poll.h>

#include "../common/Constants.h"
#include "../common/MessageType.h"
#include "../common/Serializer.h"

Server::Server(const int port) {
	this->setup(port);
	pollfd pfd{};
	pfd.fd = STDIN_FILENO;
	pfd.events = POLLIN;
	this->pfds.push_back(pfd);
	pfd.fd = this->socket;
	pfd.events = POLLIN;
	this->pfds.push_back(pfd);
	this->parser = new Parser();
}

Server::~Server() {
	if (this->socket != -1)
		close(this->socket);
	delete parser;
}

void Server::setup(const int port) {
	this->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(this->socket == -1){
		std::cerr << "Error creating socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int one = 1;
	if(setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1){
		std::cerr << "Error chaning socket options" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->address = {};
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = htonl(INADDR_ANY);
	this->address.sin_port = htons(port);
	if(bind(this->socket, reinterpret_cast<sockaddr *>(&this->address), sizeof(this->address)) == -1){
		std::cerr << "Error binding address to a socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(listen(this->socket, SOMAXCONN) == -1){
		std::cerr << "Error marking server socket as a listening socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << ntohs(this->address.sin_port) << "..." << std::endl;
}

void Server::run() {
	char buffer[MSG_SIZE];
	while(true){
		poll(this->pfds.data(), this->pfds.size(), -1);
		if(pfds[0].revents & POLLIN){
			memset(&buffer, 0, MSG_SIZE);
			const size_t bytes_read = read(STDIN_FILENO, buffer, MSG_SIZE);
			for (const auto &client : this->clients) {
				if(client->closed == 0) {
					auto *msg = new Message();
					msg->type = Response::GAME_STARTED;
					msg->length = bytes_read;
					msg->payload = buffer;
					write(client->socket, Serializer::serialize(*msg).data(), HEADER_SIZE + bytes_read);
				}
			}
		}
		if(pfds[1].revents & POLLIN){
			sockaddr_in client_address = {};
			socklen_t s = sizeof(client_address);
			const int client_socket = accept(this->socket, reinterpret_cast<sockaddr *>(&client_address), &s);
			if(client_socket == -1){
				std::cerr << "Error accepting new client" << std::endl;
				continue;
			}
			pollfd pfd{};
			pfd.fd = client_socket;
			pfd.events = POLLIN;
			this->clients.push_back(std::make_unique<Client>(client_socket, client_address));
			this->pfds.push_back(pfd);
			std::cout << "Accepted new client - " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl;
		}
		for(size_t i = 2; i < this->clients.size() + 2; i++){
			auto &client = this->clients[i - 2];
			if(client->closed == 0 && this->pfds[i].revents & POLLIN){
				memset(&buffer, 0, MSG_SIZE);
				const size_t bytes = read(client->socket, buffer, MSG_SIZE);
				client->buffer.insert(client->buffer.end(), buffer, buffer + bytes);
				if(bytes == 0){
					pfds[i].events = 0x0;
					client->closed = 1;
					continue;
				}
				auto *msg = new Message();
				while (this->parser->parse(client->buffer, msg)) {
					std::cout << "Message from " << inet_ntoa(client->address.sin_addr) << ":" << ntohs(client->address.sin_port) << " - " << msg->payload << std::endl;
				}
			}
		}
	}
}
