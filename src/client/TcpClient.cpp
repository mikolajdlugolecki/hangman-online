#include "TcpClient.h"

#include "Message.h"
#include "MessageType.h"
#include "Serializer.h"

#include <sys/socket.h>

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TcpClient::onTimeout);
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

void TcpClient::connectToServer(const QString &host, quint16 port)
{
    timer->start(3000);
    socket->connectToHost(host, port);
}

void TcpClient::onConnected()
{
    timer->stop();
    emit connected();
}

void TcpClient::onTimeout()
{
    emit timeout();
}

void TcpClient::onDisconnected()
{
    emit connectionLost();
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
    QByteArray data = socket->readAll();
    buffer.insert(buffer.end(), data.begin(), data.end());
    Message *message = new Message();

    while (this->parser->parse(buffer, message))
    {
        switch (message->type)
        {
        case ServerMessageTypes::LOGIN_OK:
            emit nicknameOK();
            break;
        case ServerMessageTypes::LOGIN_FAILED:
            emit nicknameError(QString::fromStdString(message->payload));
            break;
        case ServerMessageTypes::ROOM_CREATED:
        {
            std::vector<std::string> result = parser->splitMessage(message->payload);
            std::string roomId = result[0];
            std::string roomPin = result[1];
            emit roomCreated(QString::fromStdString(roomId), QString::fromStdString(roomPin));
        }
        break;
        case ServerMessageTypes::ROOM_OK:
            emit roomOK();
            break;
        case ServerMessageTypes::ROOM_FAILED:
            emit roomError(QString::fromStdString(message->payload));
            break;
        case ServerMessageTypes::ROOM_OWNERSHIP_TRANSFER:
            emit roomOwnershipTransfer();
            break;
        case ServerMessageTypes::ROOM_USERS_LIST:
        {
            std::vector<std::string> result = parser->splitMessage(message->payload);
            emit updateLobbyPlayerList(stdVectorToQVector(result));
        }
        break;
        case ServerMessageTypes::GAME_STARTED:
        {
            std::vector<std::string> result = parser->splitMessage(message->payload);
            std::string wordLength = result[0];
            std::string maxErrors = result[1];
            std::string maxSeconds = result[2];
            emit gameStarted(QString::fromStdString(wordLength),
                             QString::fromStdString(maxErrors),
                             QString::fromStdString(maxSeconds));
        }
        break;
        case ServerMessageTypes::PING:
            sendMessage(ClientMessageTypes::PONG, "");
            break;
        case ServerMessageTypes::GUESS_OK:
        {
            std::vector<std::string> result = parser->splitMessage(message->payload);
            emit guessPositions(result);
        }
        break;
        case ServerMessageTypes::GUESS_WRONG:
            emit guessIncorrect();
            break;
        case ServerMessageTypes::GAME_STATE:
            //std::vector<std::vector<std::string>> stats = parser->splitGameStateMessage(message->payload);
            std::vector<std::string> result = parser->splitMessage(message->payload);
            emit gameState(result);
            break;
        }
    }
    delete message;
}

void TcpClient::sendMessage(ClientMessageTypes::Type type, const QString &payload)
{
    Message *message = new Message();
    message->type = type;
    message->length = payload.length();
    message->payload = payload.toStdString();
    auto buf = Serializer::serialize(*message);
    socket->write(QByteArray::fromRawData(buf.data(), buf.size()));
    delete message;
}

void TcpClient::nicknameReceived(const QString &nickname)
{
    sendMessage(ClientMessageTypes::LOGIN, nickname);
}

void TcpClient::createRoomReceived()
{
    sendMessage(ClientMessageTypes::CREATE_ROOM, "");
}

void TcpClient::joinRoomReceived(const QString &roomId, const QString &roomPin)
{
    sendMessage(ClientMessageTypes::JOIN_ROOM, roomId + "|" + roomPin);
}

void TcpClient::leaveRoomReceived()
{
    sendMessage(ClientMessageTypes::LEAVE_ROOM, "");
}

void TcpClient::startGameReceived()
{
    sendMessage(ClientMessageTypes::START_GAME, "");
};

void TcpClient::guessReceived(const QString &letter)
{
    sendMessage(ClientMessageTypes::GUESS, letter);
};
