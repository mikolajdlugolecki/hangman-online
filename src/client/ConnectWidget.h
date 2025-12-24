#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QTcpSocket>
#include <QTimer>
#include <QWidget>

namespace Ui {
class ConnectWidget;
}

class ConnectWidget : public QWidget {
    Q_OBJECT

  public:
    explicit ConnectWidget(QWidget *parent = 0);
    ~ConnectWidget();
    void connTimedOut();

  protected:
    void connectBtnHit();

  private:
    Ui::ConnectWidget *ui;

  signals:
    void connectionRequested(const QString& host, quint16 port);
};
#endif
