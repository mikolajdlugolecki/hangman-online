#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QTimer>
#include <QWidget>
#include "../common.h"
#include <stdint.h>

namespace ReadState{
enum ReadStates {
    READ_TYPE = 0,
    READ_LENGTH,
    READ_PAYLOAD,
    NUM_OF_READ_STATES
};
}

namespace Ui {
class Client;
}

class Client : public QWidget {
    Q_OBJECT

  public:
    explicit Client(QWidget *parent = 0);
    ~Client();

  protected:
    QTcpSocket *socket = new QTcpSocket(this);
    QTimer *connTimeoutTimer = new QTimer(this);
    QByteArray buffer;
    int readState = ReadState::READ_TYPE;
    uint32_t incomingMessageLength = 0;
    void connTimedOut();
    void connectBtnHit();
    void sendBtnHit();
    void socketConnected();
    void socketDisconnected();
    void socketError();
    void socketRead();

  private:
    Ui::Client *ui;
    int construct_request(Request::Type type, const QByteArray payload);
};
#endif
