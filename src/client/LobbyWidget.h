#ifndef LOBBYWIDGET_H
#define LOBBYWIDGET_H

#include "GameState.h"

#include <QWidget>

namespace Ui
{
class LobbyWidget;
}

class LobbyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LobbyWidget(QWidget *parent = nullptr);
    ~LobbyWidget() override;
    void lobbyOwnershipReceived();
    void updateLobbyPlayerList(const QVector<QString> &nicknames);

private:
    Ui::LobbyWidget *ui;
    void showEvent(QShowEvent *event) override;
    void leaveRoomButtonHit();
    void startGameButtonHit();
    void listChanged();

signals:
    void leaveRoomRequested();
    void startGameRequested();
    void listUpdated();
};

#endif // LOBBYWIDGET_H
