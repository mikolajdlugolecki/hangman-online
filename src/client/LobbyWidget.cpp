#include "LobbyWidget.h"

#include "NonBlockingMessagebox.h"
#include "ui_LobbyWidget.h"

#include <QMessageBox>

LobbyWidget::LobbyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LobbyWidget)
{
    this->ui->setupUi(this);
    this->ui->startGameButton->setEnabled(false);
    connect(this->ui->leaveRoomButton, &QPushButton::clicked, this, &LobbyWidget::leaveRoomButtonHit);
    connect(this->ui->startGameButton, &QPushButton::clicked, this, &LobbyWidget::startGameButtonHit);
    connect(this, &LobbyWidget::listUpdated, this, &LobbyWidget::listChanged);
}

LobbyWidget::~LobbyWidget()
{
    delete this->ui;
}

void LobbyWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    this->ui->roomIdLabel->setText(GameState::instance().roomId);
    this->ui->roomPinLabel->setText(GameState::instance().roomPin);
    this->ui->startGameButton->setVisible(GameState::instance().isRoomOwner);

    updateLobbyPlayerList(GameState::instance().roomPlayers);
}

void LobbyWidget::lobbyOwnershipReceived()
{
    GameState::instance().isRoomOwner = true;
    this->ui->startGameButton->setVisible(true);
    auto *box = new NonBlockingMessageBox(this, "Info", "You are now a new owner of this room");
    box->showWithTimeout();
}

void LobbyWidget::updateLobbyPlayerList(const QVector<QString> &nicknames)
{
    this->ui->listWidget->clear();
    GameState::instance().roomPlayers.clear();

    for (const QString &nick : nicknames)
    {
        this->ui->listWidget->addItem(nick);
        GameState::instance().roomPlayers.append(nick);
    }

    emit listUpdated();
}

void LobbyWidget::listChanged()
{
    if (GameState::instance().roomPlayers.length() < 2)
    {
        this->ui->startGameButton->setEnabled(false);
        this->ui->startGameButton->setToolTip("Minimum 2 players are required to start a game");
        return;
    }
    this->ui->startGameButton->setEnabled(true);
    this->ui->startGameButton->setToolTip("");
}

void LobbyWidget::leaveRoomButtonHit()
{
    emit this->leaveRoomRequested();
}

void LobbyWidget::startGameButtonHit()
{
    emit this->startGameRequested();
}
