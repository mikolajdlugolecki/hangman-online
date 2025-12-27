#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QVector>

#include "Parser.h"
#include "MessageType.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void connectToServer(const QString& host, quint16 port);
    void nicknameReceived(const QString& nickname);
    void createRoomReceived();
    void joinRoomReceived(const QString& room_id, const QString& room_pin);
    void leaveRoomReceived();
    void startGameReceived();
protected:
    QTcpSocket *socket = new QTcpSocket(this);
    QTimer *timer = new QTimer(this);
    void onConnected();
    void onTimeout();
    void onDisconnected();
    void onReadyRead();
    void sendMessage(Request::Type type, const QString& nickname);

private:
    Parser *parser = new Parser();
    std::vector<char> buffer;

signals:
    void connected();
    void timeout();
    void connectionLost();
    void nicknameOK();
    void nicknameError(const QString error);
    void roomCreated(const QString room_id, const QString room_pin);
    void roomOK();
    void roomError(const QString error);
    void roomOwnerShipTransfer();
    void updateLobbyPlayerList(const QVector<QString> nicknames);
    void gameStarted(const QString word_length, const QString max_errors);
};

#endif // TCPCLIENT_H
