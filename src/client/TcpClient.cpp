#include "TcpClient.h"

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
        }
    }
    delete message;
}

void TcpClient::nicknameReceived(const QString& nickname){
    // send request to server
    Message *message = new Message();
    message->type = Request::LOGIN;
    message->length = nickname.length();
    message->payload = nickname.toStdString();
    auto buf = Serializer::serialize(*message);
    socket->write(QByteArray::fromRawData(buf.data(), buf.size()));
    delete message;
}
