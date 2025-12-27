#include "Server.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>

#include "Constants.h"
#include "Serializer.h"

Server::Server(const int port) {
	this->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(this->socket == -1){
		perror("Error creating socket");
		exit(EXIT_FAILURE);
	}

	const int one = 1;
	if(setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1){
		perror("Error chaning socket options");
		exit(EXIT_FAILURE);
	}

	this->address = {};
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = htonl(INADDR_ANY);
	this->address.sin_port = htons(port);
	if(bind(this->socket, reinterpret_cast<sockaddr *>(&this->address), sizeof(this->address)) == -1){
		perror("Error binding address to a socket");
		exit(EXIT_FAILURE);
	}

	if(listen(this->socket, SOMAXCONN) == -1){
		perror("Error marking server socket as a listening socket");
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
		perror("Error accepting new client");
		return;
	}

	pollfd pfd{};
	pfd.fd = client_socket;
	pfd.events = POLLIN;
	this->clients.push_back(std::make_unique<Client>(client_socket, client_address));
	this->pfds.push_back(pfd);
	write_debug_log(clients.back().get(), "New client accepted");
}

void Server::handle_client(const size_t client_index) {

	if(this->pfds[client_index].revents & POLLIN){
		char buffer[MSG_SIZE]{};
		Client *client = this->clients[client_index - 1].get();
		const size_t bytes = read(client->socket, buffer, MSG_SIZE);
		client->buffer.insert(client->buffer.end(), buffer, buffer + bytes);
		
		if(bytes == 0){
			close(client->socket);
			write_debug_log(client, "Client disconnected");
			this->clients.erase(this->clients.begin() + client_index - 1);
			this->pfds.erase(this->pfds.begin() + client_index);
			leave_room(client);
			return;
		}

		while (this->parser->parse(client->buffer, client->message)) {
			write_debug_log(client, "Message received. type = " + std::to_string(client->message->type) + " Payload " + client->message->payload);
			// auto buf = Serializer::serialize(*client->message);
			// for (unsigned char c : buf)
			// 	std::cout << std::hex << (int)c << " ";
			// std::cout << std::endl;
			this->handle_message(client, client->message);
		}
	}
}

void Server::write_debug_log(Client *client, std::string message) {
	std::cout << client->address_to_string() << " --- " << message << std::endl;
}

Room* Server::find_room(std::string id) {
	for(auto &room : this->rooms) {
		if(room->id == id) {
			return room.get();
		}
	}
	return nullptr;
}

Room * Server::find_room(Client *client) {
	for (auto &room: this->rooms) {
		if (room->isClientInRoom(client)) {
			return room.get();
		}
	}
	return nullptr;
}

void Server::start_game(Client *client) {
	auto *room = find_room(client);

	if (room == nullptr) {
		write_debug_log(client, "Could not find the client's room");
		return;
	}

	room->start_game();
}

void Server::create_new_room(Client *client)
{
	auto room = std::make_unique<Room>(this, client);
	this->send_message(client, Response::ROOM_CREATED, room->id + "|" + room->pin);
	write_debug_log(client, "Room created ID = " + room->id + " PIN = " + room->pin);
	this->rooms.push_back(std::move(room));
}

void Server::join_room(Client *client, std::string id, std::string pin)
{
	auto room = find_room(id);

	if(room == nullptr) {
		this->send_message(client, Response::ROOM_FAILED, "Room not found. Wrong room id.");
		write_debug_log(client, "Room not found ID = " + id);
		return;
	}

	if(pin != room->pin) {
		this->send_message(client, Response::ROOM_FAILED, "Wrong pin!");
		write_debug_log(client, "Wrong room pin ID = " + room->id);
		return;
	}

	room->join(client);

	this->send_message(client, Response::ROOM_OK, "");
	room->broadcast_players_list();

	write_debug_log(client, "Joined room ID = " + room->id);
}

void Server::leave_room(Client *client) {
	auto room = find_room(client);

	if (room == nullptr) {
		write_debug_log(client, "Could not find the client's room");
		return;
	}

	auto *newOwner = room->leave(client);
	write_debug_log(client, "Client left room ID = " + room->id);

	if (newOwner != nullptr) {
		write_debug_log(newOwner, "New owner of room ID = " + room->id);
		this->send_message(newOwner, Response::ROOM_OWNERSHIP_TRANSFER, "");
	}

}

void Server::handle_message(Client *client, Message *message) {
	switch (message->type) {
		case Request::LOGIN:
			if (validate_nickname(client, message->payload))
				this->send_message(client, Response::LOGIN_OK, "");
			else
				this->send_message(client, Response::LOGIN_FAILED, "Nickname is already in use");
			break;
		case Request::CREATE_ROOM:
			create_new_room(client);
			break;
		case Request::JOIN_ROOM: {
			std::vector<std::string> result = parser->split_message(message->payload);
            std::string room_id = result[0];
            std::string room_pin = result[1];
			join_room(client, room_id, room_pin);
		}
			break;
		case Request::LEAVE_ROOM:
			leave_room(client);
			break;
		case Request::START_GAME:
			start_game(client);
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
