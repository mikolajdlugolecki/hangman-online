#ifndef LOBBYWIDGET_H
#define LOBBYWIDGET_H

#include <QWidget>

namespace Ui {
class LobbyWidget;
}

class LobbyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LobbyWidget(QWidget *parent = nullptr);

private:
    Ui::LobbyWidget *ui;

signals:

};

#endif // LOBBYWIDGET_H
