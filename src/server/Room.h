#ifndef ROOM_H
#define ROOM_H

#include <random>
#include <vector>

#include "Client.h"
#include "MessageType.h"
#include "Parser.h"

class Game;
class Server;

class Room {
public:

    std::string id;
    std::string pin;
    Game* game;
    bool is_game_started = false;

    Room(Server *server, Client *owner);
    ~Room();
    void add_client(Client client);
    void broadcast_message(Response::Type type, std::string payload);
    void broadcast_players_list_lobby();
    void broadcast_players_list_game();
    void join(Client *client);
    Client* leave(Client *client);
    bool isClientInRoom(Client *client);
    void start_game();

private:
    Client *owner;
    Server *server;
    std::vector<Client*> clients;
    static inline int room_counter = 1;

    std::string generate_pin();
};  

#endif
