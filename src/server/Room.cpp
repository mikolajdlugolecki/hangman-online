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

static std::string pad_left(const std::string& str, size_t totalWidth, char padChar = ' ') {
    if (str.length() >= totalWidth) return str;
    return std::string(totalWidth - str.length(), padChar) + str;
}

static std::string pad_right(const std::string& str, size_t totalWidth, char padChar = ' ') {
    if (str.length() >= totalWidth) return str;
    return str + std::string(totalWidth - str.length(), padChar);
}


void Room::broadcast_players_list_lobby() {
    std::string message = "";

    size_t max_nickname_size = 0;
    for(auto &c : clients) {
        if(c->nickname.size()  > max_nickname_size) {
            max_nickname_size = c->nickname.size();
        }
    }

    int size = clients.size();
    for(int i = 0; i < size; i++) {
        auto client = clients[i];
        message += pad_right(client->nickname, max_nickname_size) + " (" + (client->is_connected ? "connected" : "disconnected") + ")";
        if(i != size - 1) {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcast_message(Response::ROOM_USERS_LIST, message);
}

void Room::broadcast_players_list_game() {
    std::string message = "";

    size_t max_nickname_size = 0;
    for(auto &c : clients) {
        if(c->nickname.size()  > max_nickname_size) {
            max_nickname_size = c->nickname.size();
        }
    }

    int size = clients.size();
    for(int i = 0; i < size; i++) {
        auto client = clients[i];
        //TODO: implement sending game stats for each user
        message += pad_right(client->nickname, max_nickname_size)  + " (" + (client->is_connected ? "connected" : "disconnected") + ")";
        if(i != size - 1) {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcast_message(Response::ROOM_USERS_LIST, message);
}


void Room::join(Client *client) {
    this->clients.push_back(client);
    client->room = this;
    broadcast_players_list_lobby();
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

    if(is_game_started) {
        broadcast_players_list_game();
    }
    else {
        broadcast_players_list_lobby();
    }

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
    is_game_started = true;
    broadcast_message(Response::GAME_STARTED, this->game->getGameStartedPayload());
}
