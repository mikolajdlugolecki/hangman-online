#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "Parser.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void connectToServer(const QString& host, quint16 port);
    void nicknameReceived(const QString& nickname);
protected:
    QTcpSocket *socket = new QTcpSocket(this);
    QTimer *timer = new QTimer(this);
    void onConnected();
    void onTimeout();
    void onDisconnected();
    void onReadyRead();
private:
    Parser *parser = new Parser();
    std::vector<char> buffer;

signals:
    void connected();
    void timeout();
    void nicknameOK();
    void nicknameError(const QString error);
};

#endif // TCPCLIENT_H
