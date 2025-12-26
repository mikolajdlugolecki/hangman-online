#include "Room.h"
#include "Server.h"

Room::Room(Server *server, Client *owner) {
    this->server = server;
    this->owner = owner;
    this->clients.push_back(owner);
    this->id = std::to_string(this->room_counter++);
    this->pin = generate_pin();
}

Room::~Room() {

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
