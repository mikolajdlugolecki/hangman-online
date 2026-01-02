#include "Room.h"

#include "Game.h"
#include "Server.h"

#include <algorithm>
#include <iostream>
#include <random>

Room::Room(Server *server, Client *owner)
{
    owner->room = this;
    this->server = server;
    this->owner = owner;
    this->clients.push_back(owner);
    this->id = std::to_string(Room::roomCounter++);
    this->pin = generatePin();
}

Room::~Room()
{
    this->server = nullptr;
    this->owner = nullptr;
    this->clients.clear();
}

std::string Room::generatePin()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);

    const int number = dis(gen);
    return std::to_string(number);
}

std::string Room::getGameResult()
{
    std::string result;

    std::vector<std::pair<Client *, std::shared_ptr<GameStats>>> vec(this->gameStats.begin(), this->gameStats.end());

    std::sort(vec.begin(),
              vec.end(),
              [](const auto &a, const auto &b)
              {
                  if (a.second.get()->score != b.second.get()->score)
                  {
                      return a.second.get()->score > b.second.get()->score;
                  }
                  return a.second.get()->errors < b.second.get()->errors;
              });

    size_t limit;

    if (vec.size() == 2)
    {
        limit = 2;
    }
    else
    {
        limit = 3;
    }

    for (size_t i = 0; i < limit; i++)
    {
        const auto &[client, stats] = vec[i];
        // TODO: if client left during game, there is a heap user after free warning
        result += std::to_string(i + 1) + ". " + client->nickname + " - " + std::to_string(stats.get()->score) +
                  " points - " + std::to_string(stats.get()->errors) + " errors\n";
        if (static_cast<int>(i) != std::min(2, static_cast<int>(vec.size()) - 1))
        {
            result += "|";
        }
    }

    return result;
}

void Room::broadcastMessage(const ServerMessageTypes::Type type, const std::string &payload) const
{
    for (const auto client : this->clients)
    {
        client->addMessageToBuffer(type, payload);
    }
}

// static std::string padLeft(const std::string &str, const size_t totalWidth, char padChar = ' ')
// {
//     if (str.length() >= totalWidth)
//     {
//         return str;
//     }
//     return std::string(totalWidth - str.length(), padChar) + str;
// }

static std::string padRight(const std::string &str, const size_t totalWidth, char padChar = ' ')
{
    if (str.length() >= totalWidth)
    {
        return str;
    }
    return str + std::string(totalWidth - str.length(), padChar);
}

void Room::broadcastPlayersListLobby() const
{
    std::string message;

    size_t maxNicknameSize = 0;
    for (const auto &client : this->clients)
    {
        if (client->nickname.size() > maxNicknameSize)
        {
            maxNicknameSize = client->nickname.size();
        }
    }

    const size_t size = this->clients.size();
    for (size_t i = 0; i < size; i++)
    {
        const auto client = this->clients[i];
        message += padRight(client->nickname, maxNicknameSize) + " (" +
                   (client->isConnected ? "connected" : "disconnected") + ")";
        if (i != size - 1)
        {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcastMessage(ServerMessageTypes::ROOM_USERS_LIST, message);
}

void Room::broadcastPlayersListGame() const
{
    std::string message;

    size_t maxNicknameSize = 0;
    for (const auto &client : this->clients)
    {
        if (client->nickname.size() > maxNicknameSize)
        {
            maxNicknameSize = client->nickname.size();
        }
    }

    const size_t size = this->clients.size();
    for (size_t i = 0; i < size; i++)
    {
        const auto client = this->clients[i];
        message += padRight(client->nickname, maxNicknameSize) + " (" +
                   (client->isConnected ? "connected" : "disconnected") + ")";
        if (i != size - 1)
        {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcastMessage(ServerMessageTypes::ROOM_USERS_LIST, message);
}

void Room::broadcastPlayersGameStats() const
{
    std::string payload;

    size_t maxNicknameSize = 8;
    for (const auto &client : this->clients)
    {
        if (client->nickname.size() > maxNicknameSize)
        {
            maxNicknameSize = client->nickname.size();
        }
    }

    payload += padRight("Nick", maxNicknameSize) + padRight("Score", 8) + padRight("Errors", 8) + "Connection|";

    for (size_t i = 0; i < this->clients.size(); i++)
    {
        const auto client = this->clients[i];

        const auto connectionState = (client->isConnected ? client->inGame ? "in game" : "finished" : "disconnected");

        std::shared_ptr<GameStats> stats = this->gameStats.at(client);

        payload += padRight(client->nickname, maxNicknameSize) +
                   padRight(std::to_string(stats.get()->score) + "/100", 8) +
                   padRight(std::to_string(stats.get()->errors) + "/" + std::to_string(this->game->maxErrors), 8) +
                   connectionState;

        if (i != this->clients.size() - 1)
        {
            payload += "|";
        }
    }

    std::cout << "Broadcasting game stats for room ID = " << this->id << " list = " << payload << std::endl;
    this->broadcastMessage(ServerMessageTypes::GAME_STATE, payload);
}

void Room::join(Client *client)
{
    Client *disconnectedClient = nullptr;
    for(size_t i = 0; i < disconnectedClients.size(); i++)
    {
        auto current = disconnectedClients[i];
        if(current.get()->nickname == client->nickname)
        {
            disconnectedClient = current.get();
            disconnectedClients.erase(disconnectedClients.begin() + i);
            break;
        }
    }

    this->clients.push_back(client);
    client->room = this;
    broadcastPlayersListLobby();

    if(disconnectedClient != nullptr)
    {
        client->inGame = true;
        this->gameStats[client] = gameStats[disconnectedClient];
        gameStats.erase(disconnectedClient);
        client->addMessageToBuffer(ServerMessageTypes::GAME_STARTED, this->game->getGameStartedPayload());
        broadcastPlayersListGame();
        broadcastPlayersGameStats();

        auto stats = this->gameStats[client];
        std::string payload = std::to_string(stats->errors) + "|" + 
        std::to_string(stats->score) + "|" +  
        stats->wordWithHiddenChars + "|" +
        stats->usedCharactersToString();
        client->addMessageToBuffer(ServerMessageTypes::GAME_REJOINED, payload);
    }
}

Client *Room::leave(const std::shared_ptr<Client> clientShared)
{
    auto client = clientShared.get();

    const auto iterator = std::find(this->clients.begin(), this->clients.end(), client);
    if (iterator == this->clients.end())
    {
        return nullptr;
    }

    disconnectedClients.push_back(clientShared);

    const bool wasOwner = (client == this->owner);
    this->clients.erase(iterator);

    if (this->clients.empty())
    {
        // TODO: close room, remove from server's list
        return nullptr;
    }

    if (wasOwner)
    {
        this->owner = this->clients.front();
    }

    if (this->game && this->game->inProgress)
    {
        broadcastPlayersListGame();
        broadcastPlayersGameStats();
    }
    else
    {
        broadcastPlayersListLobby();
    }

    return wasOwner ? this->owner : nullptr;
}

bool Room::isClientInRoom(const Client *client) const
{
    for (auto &roomClient : this->clients)
    {
        if (roomClient == client)
        {
            return true;
        }
    }
    return false;
}

void Room::startGame()
{
    disconnectedClients.clear();

    this->game = std::make_unique<Game>(10, 60 * 5);

    this->gameStats.clear();
    for (auto client : this->clients)
    {
        client->inGame = true;
        this->gameStats[client] = std::make_shared<GameStats>(this, this->game->word);
    }

    this->game->start();

    broadcastMessage(ServerMessageTypes::GAME_STARTED, this->game->getGameStartedPayload());

    broadcastPlayersGameStats();

    this->lastUpdate = std::chrono::steady_clock::now();
}

void Room::updateGame()
{
    const auto now = std::chrono::steady_clock::now();

    if (now - this->lastUpdate < std::chrono::seconds(1))
    {
        return;
    }
    this->game->update();

    broadcastMessage(ServerMessageTypes::REMAINING_TIME,
                     std::to_string(static_cast<int>(this->game->getRemainingTime().count())));

    if (!this->game->inProgress)
    {
        broadcastMessage(ServerMessageTypes::ROUND_TIMEOUT, this->getGameResult());
    }

    if (this->allClientsFinished())
    {
        broadcastMessage(ServerMessageTypes::ROUND_ALL_FINISHED, this->getGameResult());
        this->game->stop();
    }

    this->lastUpdate = now;
}

bool Room::allClientsFinished() const
{
    for (const auto client : this->clients)
    {
        if (client->inGame)
        {
            return false;
        }
    }
    return true;
}

std::string Room::getStats(Client *client)
{
    return std::to_string(this->gameStats[client]->score) + "|" + std::to_string(this->gameStats[client]->errors);
}
