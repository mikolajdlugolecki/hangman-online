#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "MessageType.h"
#include "Parser.h"

#include <QTcpSocket>
#include <QTimer>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient() override;
    void connectToServer(const QString &host, quint16 port) const;
    void nicknameReceived(const QString &nickname) const;
    void createRoomReceived() const;
    void joinRoomReceived(const QString &roomId, const QString &roomPin) const;
    void leaveRoomReceived() const;
    void startGameReceived() const;
    void guessReceived(const QString &letter) const;

protected:
    QTcpSocket *socket = new QTcpSocket(this);
    QTimer *timer = new QTimer(this);
    void onConnected();
    void onTimeout();
    void onDisconnected();
    void onReadyRead();
    void sendMessage(ClientMessageTypes::Type type, const QString &payload) const;

private:
    Parser *parser = new Parser();
    std::vector<char> buffer;

signals:
    void connected();
    void timeout();
    void connectionLost();
    void nicknameOK();
    void nicknameError(QString error);
    void roomCreated(QString roomId, QString roomPin);
    void roomOK();
    void roomError(QString error);
    void roomOwnershipTransfer();
    void updateLobbyPlayerList(QVector<QString> nicknames);
    void gameStarted(QString wordLength, QString maxErrors, QString maxSeconds, QString coveredWord);
    void gameRejoined(QString errors, QString score, QString word, QString usedCharacters);
    void gameState(std::vector<std::string> stats);
    void guessCorrect(QString newWordWithHiddenChars, QString currentScore);
    void guessIncorrect(QString currentScore);
    void gameRemainingTime(QString seconds);
    void roundOver(int type, std::vector<std::string> payload);
};

#endif // TCPCLIENT_H
