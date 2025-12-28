#include "LobbyWidget.h"
#include "ui_LobbyWidget.h"

#include <QMessageBox>

LobbyWidget::LobbyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LobbyWidget)
{
    ui->setupUi(this);
    connect(ui->leaveRoomButton, &QPushButton::clicked, this, &LobbyWidget::leaveRoomButtonHit);
    connect(ui->startGameButton, &QPushButton::clicked, this, &LobbyWidget::startGameButtonHit);
}

LobbyWidget::~LobbyWidget()
{
    delete ui;
}

void LobbyWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    ui->roomIdLabel->setText(GameState::instance().roomId);
    ui->roomPinLabel->setText(GameState::instance().roomPin);
    ui->startGameButton->setVisible(GameState::instance().isRoomOwner);

    updateLobbyPlayerList(GameState::instance().roomPlayers);
}

void LobbyWidget::lobbyOwnershipReceived()
{
    GameState::instance().isRoomOwner = true;
    ui->startGameButton->setVisible(true);
    QMessageBox::information(this, "Info", "You are now a new owner of this room");
}

void LobbyWidget::updateLobbyPlayerList(QVector<QString> nicknames)
{
    ui->listWidget->clear();
    GameState::instance().roomPlayers.clear();

    for (const QString &nick : nicknames)
    {
        ui->listWidget->addItem(nick);
    }
}

void LobbyWidget::leaveRoomButtonHit()
{
    emit leaveRoomRequested();
}

void LobbyWidget::startGameButtonHit()
{
    emit startGameRequested();
}
