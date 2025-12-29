#include "LobbyWidget.h"

#include "ui_LobbyWidget.h"

#include <QMessageBox>

LobbyWidget::LobbyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LobbyWidget)
{
    this->ui->setupUi(this);
    connect(this->ui->leaveRoomButton, &QPushButton::clicked, this, &LobbyWidget::leaveRoomButtonHit);
    connect(this->ui->startGameButton, &QPushButton::clicked, this, &LobbyWidget::startGameButtonHit);
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
    QMessageBox::information(this, "Info", "You are now a new owner of this room");
}

void LobbyWidget::updateLobbyPlayerList(const QVector<QString> &nicknames)
{
    this->ui->listWidget->clear();
    GameState::instance().roomPlayers.clear();

    for (const QString &nick : nicknames)
    {
        this->ui->listWidget->addItem(nick);
    }
}

void LobbyWidget::leaveRoomButtonHit()
{
    emit this->leaveRoomRequested();
}

void LobbyWidget::startGameButtonHit()
{
    emit this->startGameRequested();
}
