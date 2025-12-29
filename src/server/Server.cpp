#include "Server.h"

#include "Constants.h"
#include "Game.h"
#include "Serializer.h"

#include <cstdlib>
#include <fcntl.h>
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
            client->tick();
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
    if (fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL) | O_NONBLOCK) == -1)
    {
        perror("Error changing client's socket settings");
        return;
    }
    pollfd pfd{};
    pfd.fd = clientSocket;
    pfd.events = POLLIN | POLLOUT;
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
        client->receivingBuffer.insert(client->receivingBuffer.end(), buffer, buffer + bytes);

        if (bytes == 0)
        {
            close(client->socket);
            leaveRoom(client);
            Utils::writeDebugLog(client, "Client disconnected");
            this->clients.erase(this->clients.begin() + client_index - 1);
            this->pfds.erase(this->pfds.begin() + client_index);
            return;
        }

        while (this->parser->parse(client->receivingBuffer, client->message))
        {
            if (client->message->type != ClientMessageTypes::PONG)
            {
                Utils::writeDebugLog(client,
                                     "Message received. type = " + std::to_string(client->message->type) + " Payload " +
                                         client->message->payload);
                // Utils::writeMessageAsHex(client->message);
            }
            this->handleMessage(client, client->message);
        }
    }

    if (this->pfds[client_index].revents & POLLOUT)
    {
        Client *client = this->clients[client_index - 1].get();
        this->sendBufferData(client);
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

void Server::checkGuess(Client *client, const char &letter)
{
    const auto *room = client->room;

    if (room == nullptr)
    {
        Utils::writeDebugLog(client, "Could not find the client's room");
        return;
    }

    auto stats = room->gameStats.at(client).get();

	if(stats->errors >= room->game->maxErrors)
	{
		return;
	}

	if(stats->isLetterUsed(letter))
	{
		return;
	}
	stats->markLetterAsUsed(letter);

    if (stats->isLetterCorrect(letter))
    {
		stats->letterGuessed(room->game->word, letter);

        client->addMessageToBuffer(ServerMessageTypes::GUESS_OK, stats->wordWithHiddenChars);
    }
    else
    {
		stats->errors++;
        client->addMessageToBuffer(ServerMessageTypes::GUESS_WRONG, "");
    }

    stats->recalculateScore();
    room->broadcastPlayersGameStats();
}

void Server::createNewRoom(Client *client)
{
    auto room = std::make_unique<Room>(this, client);
    client->addMessageToBuffer(ServerMessageTypes::ROOM_CREATED, room->id + "|" + room->pin);
    Utils::writeDebugLog(client, "Room created ID = " + room->id + " PIN = " + room->pin);
    this->rooms.push_back(std::move(room));
}

void Server::joinRoom(Client *client, const std::string &id, const std::string &pin)
{
    const auto room = Utils::findRoom(rooms, id);

    if (room == nullptr)
    {
        client->addMessageToBuffer(ServerMessageTypes::ROOM_FAILED, "Room not found. Wrong room id.");
        Utils::writeDebugLog(client, "Room not found ID = " + id);
        return;
    }

    if (pin != room->pin)
    {
        client->addMessageToBuffer(ServerMessageTypes::ROOM_FAILED, "Wrong pin!");
        Utils::writeDebugLog(client, "Wrong room pin ID = " + room->id);
        return;
    }

    room->join(client);
    client->addMessageToBuffer(ServerMessageTypes::ROOM_OK, "");

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

    auto *newOwner = room->leave(client);
    Utils::writeDebugLog(client, "Client left room ID = " + room->id);

    if (newOwner != nullptr)
    {
        Utils::writeDebugLog(newOwner, "New owner of room ID = " + room->id);
        newOwner->addMessageToBuffer(ServerMessageTypes::ROOM_OWNERSHIP_TRANSFER, "");
    }
}

void Server::handleMessage(Client *client, const Message *message)
{
    switch (message->type)
    {
    case ClientMessageTypes::LOGIN:
        if (validateNickname(client, message->payload))
        {
            client->addMessageToBuffer(ServerMessageTypes::LOGIN_OK, "");
        }
        else
        {
            client->addMessageToBuffer(ServerMessageTypes::LOGIN_FAILED, "Nickname is already in use");
        }
        break;
    case ClientMessageTypes::CREATE_ROOM:
        createNewRoom(client);
        break;
    case ClientMessageTypes::JOIN_ROOM:
    {
        const std::vector<std::string> result = Parser::splitMessage(message->payload);
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
	{
		char letter = std::toupper((message->payload)[0]);
        checkGuess(client, letter);
	}    
    default:
        break;
    }
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

void Server::sendBufferData(Client *client) const
{
    const size_t minSize = std::min(static_cast<size_t>(MSG_SIZE), client->sendingBuffer.size());
    const auto buffer = new char[minSize];
    for (size_t i = 0; i < minSize; i++)
    {
        buffer[i] = client->sendingBuffer[i];
    }
    const size_t bytesSent = write(client->socket, buffer, minSize);
    for (size_t i = 0; i < bytesSent; i++)
    {
        client->sendingBuffer.pop_front();
    }
    delete[] buffer;
}
