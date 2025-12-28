#include "Client.h"

#include <arpa/inet.h>

#include "Server.h"
#include "Room.h"

Client::Client(const int in_socket, const sockaddr_in in_address) {
    this->socket = in_socket;
    this->address = in_address;
    this->message = new Message();
}

Client::~Client() {
    delete message;
}

std::string Client::address_to_string()
{
    return std::string(inet_ntoa(this->address.sin_addr)) + ":" + std::to_string(ntohs(this->address.sin_port));   
}

void Client::tick(Server *server) {
    ping_interval_counter_seconds--;
    if(ping_interval_counter_seconds <= 0) {
        ping_interval_counter_seconds = PING_INTERVAL_SECONDS;
        pong_timeout_counters_seconds.push_back(PONG_TIMEOUT);
        //server->write_debug_log(this, "Sending ping");
        server->send_message(this, Response::PING, "");
    }

    for(auto  &p : pong_timeout_counters_seconds) {
        p--;
    }
    
    if(pong_timeout_counters_seconds.size() > 0 && pong_timeout_counters_seconds[0] <= 0) {
        pong_timeout_counters_seconds.erase(pong_timeout_counters_seconds.begin());
        received_pong = false;
    }

    if(is_connected != received_pong && room != nullptr) {
        is_connected = received_pong;

        if(room->is_game_started == false) {
            room->broadcast_players_list_lobby();
        }
        else {
            room->broadcast_players_list_game();
        }
    }
        
}