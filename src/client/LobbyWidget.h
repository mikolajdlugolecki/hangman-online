#ifndef LOBBYWIDGET_H
#define LOBBYWIDGET_H

#include <QWidget>
#include "GameState.h"

namespace Ui {
class LobbyWidget;
}

class LobbyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LobbyWidget(QWidget *parent = nullptr);
    ~LobbyWidget();

    void updateLobbyPlayerList(QVector<QString> nicknames);

private:
    Ui::LobbyWidget *ui;
    void showEvent(QShowEvent *event) override;
};

#endif // LOBBYWIDGET_H
