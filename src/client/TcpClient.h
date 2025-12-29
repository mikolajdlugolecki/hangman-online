#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "MessageType.h"
#include "Parser.h"

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QVector>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void nicknameReceived(const QString &nickname);
    void createRoomReceived();
    void joinRoomReceived(const QString &roomId, const QString &roomPin);
    void leaveRoomReceived();
    void startGameReceived();
    void guessReceived(const QString &letter);

protected:
    QTcpSocket *socket = new QTcpSocket(this);
    QTimer *timer = new QTimer(this);
    void onConnected();
    void onTimeout();
    void onDisconnected();
    void onReadyRead();
    void sendMessage(ClientMessageTypes::Type type, const QString &nickname);

private:
    Parser *parser = new Parser();
    std::vector<char> buffer;

signals:
    void connected();
    void timeout();
    void connectionLost();
    void nicknameOK();
    void nicknameError(const QString error);
    void roomCreated(const QString roomId, const QString roomPin);
    void roomOK();
    void roomError(const QString error);
    void roomOwnershipTransfer();
    void updateLobbyPlayerList(const QVector<QString> nicknames);
    void gameStarted(const QString wordLength, const QString maxErrors, const QString maxSeconds, const QString coveredWord);
    void gameState(const std::vector<std::string> stats);
    void guessCorrect(QString newWordWithHiddenChars);
    void guessIncorrect();
};

#endif // TCPCLIENT_H
