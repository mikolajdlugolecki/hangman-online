#include "Room.h"

#include <algorithm>
#include <iostream>

#include "Server.h"
#include "Game.h"

Room::Room(Server *server, Client *owner) {
    this->server = server;
    this->owner = owner;
    this->clients.push_back(owner);
    this->id = std::to_string(Room::room_counter++);
    this->pin = generate_pin();
}

Room::~Room() {
    this->server = nullptr;
    this->owner = nullptr;
    this->clients.clear();
}

std::string Room::generate_pin() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    int number = dis(gen);
    return std::to_string(number);
}

void Room::broadcast_message(Response::Type type, std::string payload) {
    for(auto c : this->clients) {
        this->server->send_message(c, type, payload);
    }
}

void Room::broadcast_players_list() {
    std::string message = "";

    int size = clients.size();
    for(int i = 0; i < size; i++) {
        message += clients[i]->nickname;
        if(i != size - 1) {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcast_message(Response::ROOM_USERS_LIST, message);
}

void Room::join(Client *client) {
    this->clients.push_back(client);
    broadcast_players_list();
}

Client* Room::leave(Client* client) {
    const auto iterator = std::find(clients.begin(), clients.end(), client);
    if (iterator == clients.end()) {
        return nullptr;
    }

    bool wasOwner = (client == owner);
    clients.erase(iterator);

    if (clients.empty()) {
        // TODO: close room, remove from server's list
        return nullptr;
    }

    if (wasOwner) {
        owner = clients.front();
    }

    broadcast_players_list();
    return wasOwner ? owner : nullptr;
}

bool Room::isClientInRoom(Client *client) {
    for (auto &roomClient : this->clients) {
        if(roomClient == client) {
            return true;
        }
    }
    return false;
}

void Room::start_game() {
    this->game = Game::create(10);
    broadcast_message(Response::GAME_STARTED, this->game->getGameStartedPayload());
}
