#include "Server.h"

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "Constants.h"
#include "Serializer.h"

Server::Server(const int port) {
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
	pollfd pfd{};
	pfd.fd = this->socket;
	pfd.events = POLLIN;
	this->pfds.push_back(pfd);
	this->parser = new Parser();
	std::cout << "Server listening on port " << ntohs(this->address.sin_port) << "..." << std::endl << std::endl;
}

Server::~Server() {
	if (this->socket != -1)
		close(this->socket);
	delete parser;
}

void Server::accept_new_client() {
	sockaddr_in client_address = {};
	socklen_t s = sizeof(client_address);
	const int client_socket = accept(this->socket, reinterpret_cast<sockaddr *>(&client_address), &s);
	if(client_socket == -1){
		std::cerr << "Error accepting new client" << std::endl;
		return;
	}
	pollfd pfd{};
	pfd.fd = client_socket;
	pfd.events = POLLIN;
	this->clients.push_back(std::make_unique<Client>(client_socket, client_address));
	this->pfds.push_back(pfd);
	std::cout << "Accepted new client - " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl;
}

void Server::handle_client(const size_t client_index) {

	if(this->pfds[client_index].revents & POLLIN){
		char buffer[MSG_SIZE]{};
		Client *client = this->clients[client_index - 1].get();
		const size_t bytes = read(client->socket, buffer, MSG_SIZE);
		client->buffer.insert(client->buffer.end(), buffer, buffer + bytes);
		if(bytes == 0){
			close(client->socket);
			std::cout << "Client disconnected - " << inet_ntoa(client->address.sin_addr) << ":" << ntohs(client->address.sin_port) << std::endl;
			this->clients.erase(this->clients.begin() + client_index - 1);
			this->pfds.erase(this->pfds.begin() + client_index);
			return;
		}
		while (this->parser->parse(client->buffer, client->message)) {
			std::cout << "Message from " << inet_ntoa(client->address.sin_addr) << ":" << ntohs(client->address.sin_port) << " - " << client->message->payload << std::endl;
			// auto buf = Serializer::serialize(*client->message);
			// for (unsigned char c : buf)
			// 	std::cout << std::hex << (int)c << " ";
			// std::cout << std::endl;
			this->handle_message(client, client->message);
		}
	}
}

void Server::handle_message(Client *client, Message *message) {
	switch (message->type) {
		case Request::LOGIN:
			if (validate_nickname(client, message->payload))
				this->send_message(client, Response::LOGIN_OK, "");
			else
				this->send_message(client, Response::LOGIN_FAILED, "Nickname is already taken");
			break;
		default:
			break;
	}
}

void Server::send_message(const Client *client, Response::Type type, const std::string& payload) {
	client->message->type = type;
	client->message->length = payload.size();
	client->message->payload = payload;
	// auto buf = Serializer::serialize(*client->message);
	// for (unsigned char c : buf)
	// 	std::cout << std::hex << (int)c << " ";
	// std::cout << std::endl;
	write(client->socket, Serializer::serialize(*client->message).data(), HEADER_SIZE + client->message->length);
}

bool Server::validate_nickname(Client *client, const std::string &nickname) {
	for (auto &c : this->clients) {
		if (c->nickname == nickname)
			return false;
	}
	client->nickname = nickname;
	return true;
}

void Server::run() {
	while(true) {
		poll(this->pfds.data(), this->pfds.size(), -1);
		if (pfds[0].revents & POLLIN)
			this->accept_new_client();
		for (size_t i = 1; i < this->clients.size() + 1; i++)
			this->handle_client(i);
	}
}
