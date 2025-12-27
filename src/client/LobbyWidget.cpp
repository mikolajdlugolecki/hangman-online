#include "LobbyWidget.h"
#include "ui_LobbyWidget.h"

LobbyWidget::LobbyWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LobbyWidget)
{
    ui->setupUi(this);
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

void LobbyWidget::updateLobbyPlayerList(QVector<QString> nicknames) {
    ui->listWidget->clear();
    GameState::instance().roomPlayers.clear();

    for (const QString &nick : nicknames) {
        ui->listWidget->addItem(nick);
    }
}
