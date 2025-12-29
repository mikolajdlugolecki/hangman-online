#include "TcpClient.h"

#include "Message.h"
#include "MessageType.h"
#include "Serializer.h"

#include <sys/socket.h>

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    this->timer->setSingleShot(true);
    connect(this->timer, &QTimer::timeout, this, &TcpClient::onTimeout);
    connect(this->socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(this->socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(this->socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

TcpClient::~TcpClient()
{
    delete this->parser;
}

void TcpClient::connectToServer(const QString &host, const quint16 port) const
{
    this->timer->start(3000);
    this->socket->connectToHost(host, port);
}

void TcpClient::onConnected()
{
    this->timer->stop();
    emit this->connected();
}

void TcpClient::onTimeout()
{
    emit this->timeout();
}

void TcpClient::onDisconnected()
{
    emit this->connectionLost();
}

static QVector<QString> stdVectorToQVector(const std::vector<std::string> &stdStrings)
{
    QVector<QString> qStrings;
    for (const auto &s : stdStrings)
    {
        qStrings.append(QString::fromStdString(s));
    }

    return qStrings;
}

void TcpClient::onReadyRead()
{
    QByteArray data = this->socket->readAll();
    this->buffer.insert(this->buffer.end(), data.begin(), data.end());
    auto *message = new Message();

    while (this->parser->parse(this->buffer, message))
    {
        switch (message->type)
        {
        case ServerMessageTypes::LOGIN_OK:
            emit this->nicknameOK();
            break;
        case ServerMessageTypes::LOGIN_FAILED:
            emit this->nicknameError(QString::fromStdString(message->payload));
            break;
        case ServerMessageTypes::ROOM_CREATED:
        {
            std::vector<std::string> result = Parser::splitMessage(message->payload);
            const std::string &roomId = result[0];
            const std::string &roomPin = result[1];
            emit this->roomCreated(QString::fromStdString(roomId), QString::fromStdString(roomPin));
        }
        break;
        case ServerMessageTypes::ROOM_OK:
            emit this->roomOK();
            break;
        case ServerMessageTypes::ROOM_FAILED:
            emit this->roomError(QString::fromStdString(message->payload));
            break;
        case ServerMessageTypes::ROOM_OWNERSHIP_TRANSFER:
            emit this->roomOwnershipTransfer();
            break;
        case ServerMessageTypes::ROOM_USERS_LIST:
        {
            std::vector<std::string> result = Parser::splitMessage(message->payload);
            emit this->updateLobbyPlayerList(stdVectorToQVector(result));
        }
        break;
        case ServerMessageTypes::GAME_STARTED:
        {
            std::vector<std::string> result = Parser::splitMessage(message->payload);
            const std::string &wordLength = result[0];
            const std::string &maxErrors = result[1];
            const std::string &maxSeconds = result[2];
            const std::string &coveredWord = result[3];
            emit this->gameStarted(QString::fromStdString(wordLength),
                                   QString::fromStdString(maxErrors),
                                   QString::fromStdString(maxSeconds),
                                   QString::fromStdString(coveredWord));
        }
        break;
        case ServerMessageTypes::PING:
            sendMessage(ClientMessageTypes::PONG, "");
            break;
        case ServerMessageTypes::GUESS_OK:
        {
            emit this->guessCorrect(QString::fromStdString(message->payload));
        }
        break;
        case ServerMessageTypes::GUESS_WRONG:
            emit this->guessIncorrect();
            break;
        case ServerMessageTypes::GAME_STATE:
            std::vector<std::string> result = Parser::splitMessage(message->payload);
            emit this->gameState(result);
            break;
        }
    }
    delete message;
}

void TcpClient::sendMessage(ClientMessageTypes::Type type, const QString &payload) const
{
    auto *message = new Message();
    message->type = type;
    message->length = payload.length();
    message->payload = payload.toStdString();
    const auto buf = Serializer::serialize(*message);
    this->socket->write(QByteArray::fromRawData(buf.data(), static_cast<int>(buf.size())));
    delete message;
}

void TcpClient::nicknameReceived(const QString &nickname) const
{
    this->sendMessage(ClientMessageTypes::LOGIN, nickname);
}

void TcpClient::createRoomReceived() const
{
    this->sendMessage(ClientMessageTypes::CREATE_ROOM, "");
}

void TcpClient::joinRoomReceived(const QString &roomId, const QString &roomPin) const
{
    this->sendMessage(ClientMessageTypes::JOIN_ROOM, roomId + "|" + roomPin);
}

void TcpClient::leaveRoomReceived() const
{
    this->sendMessage(ClientMessageTypes::LEAVE_ROOM, "");
}

void TcpClient::startGameReceived() const
{
    this->sendMessage(ClientMessageTypes::START_GAME, "");
};

void TcpClient::guessReceived(const QString &letter) const
{
    this->sendMessage(ClientMessageTypes::GUESS, letter);
};
