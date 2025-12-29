#include "Server.h"

#include "Constants.h"
#include "Game.h"
#include "Serializer.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void Server::timerThread()
{
    while (true)
    {
        sleep(1);

        for (const auto &client : this->clients)
        {
            client->tick(this);
        }
    }
}

Server::Server(const int port)
{
    this->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->socket == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    constexpr int one = 1;
    if (setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1)
    {
        perror("Error chaning socket options");
        exit(EXIT_FAILURE);
    }

    this->address = {};
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = htonl(INADDR_ANY);
    this->address.sin_port = htons(port);
    if (bind(this->socket, reinterpret_cast<sockaddr *>(&this->address), sizeof(this->address)) == -1)
    {
        perror("Error binding address to a socket");
        exit(EXIT_FAILURE);
    }

    if (listen(this->socket, SOMAXCONN) == -1)
    {
        perror("Error marking server socket as a listening socket");
        exit(EXIT_FAILURE);
    }

    pollfd pfd{};
    pfd.fd = this->socket;
    pfd.events = POLLIN;
    this->pfds.push_back(pfd);

    this->parser = new Parser();

    std::thread timer([this]() { timerThread(); });
    timer.detach();

    std::cout << "Server listening on port " << ntohs(this->address.sin_port) << "..." << std::endl << std::endl;
}

Server::~Server()
{
    if (this->socket != -1)
    {
        close(this->socket);
    }
    delete parser;
}

void Server::acceptNewClient()
{
    sockaddr_in clientAddress = {};
    socklen_t s = sizeof(clientAddress);

    const int clientSocket = accept(this->socket, reinterpret_cast<sockaddr *>(&clientAddress), &s);
    if (clientSocket == -1)
    {
        perror("Error accepting new client");
        return;
    }

    pollfd pfd{};
    pfd.fd = clientSocket;
    pfd.events = POLLIN;
    this->clients.push_back(std::make_unique<Client>(clientSocket, clientAddress));
    this->pfds.push_back(pfd);
    Utils::writeDebugLog(clients.back().get(), "New client accepted");
}

void Server::handleClient(const size_t client_index)
{
    if (this->pfds[client_index].revents & POLLIN)
    {
        char buffer[MSG_SIZE]{};
        Client *client = this->clients[client_index - 1].get();
        const size_t bytes = read(client->socket, buffer, MSG_SIZE);
        client->buffer.insert(client->buffer.end(), buffer, buffer + bytes);

        if (bytes == 0)
        {
            close(client->socket);
            leaveRoom(client);
            Utils::writeDebugLog(client, "Client disconnected");
            this->clients.erase(this->clients.begin() + client_index - 1);
            this->pfds.erase(this->pfds.begin() + client_index);
            return;
        }

        while (this->parser->parse(client->buffer, client->message))
        {
			if(client->message->type != ClientMessageTypes::PONG)
			{
				Utils::writeDebugLog(client,
					"Message received. type = " + std::to_string(client->message->type) + " Payload " +
					client->message->payload);
			}
            // auto buf = Serializer::serialize(*client->message);
            // for (unsigned char c : buf)
            // 	std::cout << std::hex << (int)c << " ";
            // std::cout << std::endl;
            this->handleMessage(client, client->message);
        }
    }
}

void Server::startGame(const Client *roomOwner) const
{
    auto *room = roomOwner->room;

    if (room == nullptr)
    {
        Utils::writeDebugLog(roomOwner, "Could not find the client's room");
        return;
    }

    room->startGame();
}

void Server::checkGuess(Client *client, const std::string &letter)
{
    const auto *room = client->room;

    if (room == nullptr)
    {
        Utils::writeDebugLog(client, "Could not find the client's room");
        return;
    }

	auto stats = room->gameStats.at(client).get();

    auto positions = room->game->letterInWord(letter);
    if (!positions.empty())
    {
		stats->letterGuessed(room->game->word, letter[0]);

        this->sendMessage(client, ServerMessageTypes::GUESS_OK, positions);
    }
    else
    {
		stats->errors++;

        if (++client->errors == room->game->maxErrors)
        {
        }
        this->sendMessage(client, ServerMessageTypes::GUESS_WRONG, "");
    }

	stats->recalculateScore();
	room->broadcastPlayersGameStats();
    // terminate called after throwing an instance of 'std::length_error'
    // what():  vector::_M_range_insert
    // room->broadcastPlayersGameStats();
}

void Server::createNewRoom(Client *client)
{
    auto room = std::make_unique<Room>(this, client);
    this->sendMessage(client, ServerMessageTypes::ROOM_CREATED, room->id + "|" + room->pin);
    Utils::writeDebugLog(client, "Room created ID = " + room->id + " PIN = " + room->pin);
    this->rooms.push_back(std::move(room));
}

void Server::joinRoom(Client *client, const std::string &id, const std::string &pin)
{
    const auto room = Utils::findRoom(rooms, id);

    if (room == nullptr)
    {
        this->sendMessage(client, ServerMessageTypes::ROOM_FAILED, "Room not found. Wrong room id.");
        Utils::writeDebugLog(client, "Room not found ID = " + id);
        return;
    }

    if (pin != room->pin)
    {
        this->sendMessage(client, ServerMessageTypes::ROOM_FAILED, "Wrong pin!");
        Utils::writeDebugLog(client, "Wrong room pin ID = " + room->id);
        return;
    }

    room->join(client);

    this->sendMessage(client, ServerMessageTypes::ROOM_OK, "");
    room->broadcastPlayersListLobby();

    Utils::writeDebugLog(client, "Joined room ID = " + room->id);
}

void Server::leaveRoom(const Client *client)
{
    const auto room = client->room;

    if (room == nullptr)
    {
        Utils::writeDebugLog(client, "Could not find the client's room");
        return;
    }

    const auto *newOwner = room->leave(client);
    Utils::writeDebugLog(client, "Client left room ID = " + room->id);

    if (newOwner != nullptr)
    {
        Utils::writeDebugLog(newOwner, "New owner of room ID = " + room->id);
        this->sendMessage(newOwner, ServerMessageTypes::ROOM_OWNERSHIP_TRANSFER, "");
    }
}

void Server::handleMessage(Client *client, const Message *message)
{
    switch (message->type)
    {
    case ClientMessageTypes::LOGIN:
        if (validateNickname(client, message->payload))
        {
            this->sendMessage(client, ServerMessageTypes::LOGIN_OK, "");
        }
        else
        {
            this->sendMessage(client, ServerMessageTypes::LOGIN_FAILED, "Nickname is already in use");
        }
        break;
    case ClientMessageTypes::CREATE_ROOM:
        createNewRoom(client);
        break;
    case ClientMessageTypes::JOIN_ROOM:
    {
        const std::vector<std::string> result = parser->splitMessage(message->payload);
        const std::string &room_id = result[0];
        const std::string &room_pin = result[1];
        joinRoom(client, room_id, room_pin);
    }
    break;
    case ClientMessageTypes::LEAVE_ROOM:
        leaveRoom(client);
        break;
    case ClientMessageTypes::START_GAME:
        startGame(client);
        break;
    case ClientMessageTypes::PONG:
    {
		client->pongReceived();
    }
    break;
    case ClientMessageTypes::GUESS:
        checkGuess(client, message->payload);
        break;
    default:
        break;
    }
}

void Server::sendMessage(const Client *client, const ServerMessageTypes::Type type, const std::string &payload)
{
    client->message->type = type;
    client->message->length = payload.size();
    client->message->payload = payload;
    // auto buf = Serializer::serialize(*client->message);
    // for (unsigned char c : buf)
    // 	std::cout << std::hex << (int)c << " ";
    // std::cout << std::endl;
    write(client->socket, Serializer::serialize(*client->message).data(), HEADER_SIZE + client->message->length);
}

bool Server::validateNickname(Client *client, const std::string &nickname) const
{
    for (auto &c : this->clients)
    {
        if (c->nickname == nickname)
        {
            return false;
        }
    }
    client->nickname = nickname;
    return true;
}

void Server::run()
{
    while (true)
    {
        poll(this->pfds.data(), this->pfds.size(), -1);
        if (pfds[0].revents & POLLIN)
        {
            this->acceptNewClient();
        }
        for (size_t i = 1; i < this->clients.size() + 1; i++)
        {
            this->handleClient(i);
        }
    }
}
