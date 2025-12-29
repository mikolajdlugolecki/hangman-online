#include "MenuWidget.h"

#include "ui_MenuWidget.h"

#include <QMessageBox>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    connect(ui->createRoomBtn, &QPushButton::clicked, this, &MenuWidget::createRoomButtonHit);
    connect(ui->joinRoomBtn, &QPushButton::clicked, this, &MenuWidget::joinRoomButtonHit);
    connect(ui->roomPinLineEdit, &QLineEdit::returnPressed, ui->joinRoomBtn, &QPushButton::click);
    connect(ui->roomIdLineEdit, &QLineEdit::textChanged, this, &MenuWidget::validateData);
    connect(ui->roomPinLineEdit, &QLineEdit::textChanged, this, &MenuWidget::validateData);
}

void MenuWidget::createRoomButtonHit()
{
    emit createRoomRequested();
}

void MenuWidget::roomCreated(const QString &roomId, const QString &roomPin)
{
    QMessageBox::information(this, "Room created", "Room ID: " + roomId + "\nRoom PIN: " + roomPin);
}

void MenuWidget::joinRoomButtonHit()
{
    QString roomId = ui->roomIdLineEdit->text();
    QString roomPin = ui->roomPinLineEdit->text();

    GameState::instance().roomId = roomId;
    GameState::instance().roomPin = roomPin;
    GameState::instance().isRoomOwner = false;

    emit joinRoomRequested(roomId, roomPin);
}

void MenuWidget::validateData()
{
    if (ui->roomIdLineEdit->text().length() > 0 && ui->roomPinLineEdit->text().length() == 4)
    {
        ui->joinRoomBtn->setEnabled(true);
    }
    else
    {
        ui->joinRoomBtn->setEnabled(false);
    }
}

void MenuWidget::roomError(const QString error)
{
    QMessageBox::critical(this, "Error", error);
}
