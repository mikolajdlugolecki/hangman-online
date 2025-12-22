#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QTimer>
#include <QWidget>
#include "../common/Parser.h"

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
    void connTimedOut();
    void connectBtnHit();
    void sendBtnHit();
    void socketConnected();
    void socketDisconnected();
    void socketError();
    void socketRead();
    Parser *parser;

  private:
    Ui::Client *ui;
};
#endif
