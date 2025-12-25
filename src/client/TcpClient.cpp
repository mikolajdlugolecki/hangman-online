#include "TcpClient.h"

#include "Message.h"
#include "MessageType.h"
#include "Serializer.h"
#include <sys/socket.h>

TcpClient::TcpClient(QObject *parent)
    : QObject{parent}
{
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TcpClient::onTimeout);
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

void TcpClient::connectToServer(const QString& host, quint16 port){
    timer->start(3000);
    socket->connectToHost(host, port);
}

void TcpClient::onConnected(){
    timer->disconnect();
    emit connected();
}

void TcpClient::onTimeout(){
    emit timeout();
}

void TcpClient::onDisconnected(){

}

void TcpClient::onReadyRead(){
    QByteArray data = socket->readAll();
    buffer.insert(buffer.end(), data.begin(), data.end());
    Message* message = new Message();
    while(this->parser->parse(buffer, message)){
        switch(message->type){
        case Response::LOGIN_OK:
            emit nicknameOK();
            break;
        case Response::LOGIN_FAILED:
            emit nicknameError(QString::fromStdString(message->payload));
            break;
        case Response::ROOM_CREATED:{
            std::vector<std::string> result = parser->split_message(message->payload);
            std::string room_id = result[0];
            std::string room_pin = result[1];
            emit roomCreated(QString::fromStdString(room_id), QString::fromStdString(room_pin));
            break;}
        case Response::ROOM_OK:
            emit roomOK();
            break;
        case Response::ROOM_FAILED:
            emit roomError(QString::fromStdString(message->payload));
        }
    }
    delete message;
}

void TcpClient::sendMessage(Request::Type type, const QString& payload){
    Message *message = new Message();
    message->type = type;
    message->length = payload.length();
    message->payload = payload.toStdString();
    auto buf = Serializer::serialize(*message);
    socket->write(QByteArray::fromRawData(buf.data(), buf.size()));
    delete message;
}

void TcpClient::nicknameReceived(const QString& nickname){ sendMessage(Request::LOGIN, nickname); }

void TcpClient::createRoomReceived(){ sendMessage(Request::CREATE_ROOM, ""); }

void TcpClient::joinRoomReceived(const QString& room_id, const QString& room_pin){ sendMessage(Request::JOIN_ROOM, room_id + "|" + room_pin); }

