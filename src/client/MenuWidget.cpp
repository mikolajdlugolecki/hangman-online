#include "MenuWidget.h"
#include "ui_MenuWidget.h"

#include <QMessageBox>

MenuWidget::MenuWidget(QWidget *parent)
    : QWidget{parent}, ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    connect(ui->createRoomBtn, &QPushButton::clicked, this, &MenuWidget::createRoomBtnHit);
    connect(ui->joinRoomBtn, &QPushButton::clicked, this, &MenuWidget::joinRoomBtnHit);
    connect(ui->roomPinLineEdit, &QLineEdit::returnPressed, ui->joinRoomBtn, &QPushButton::click);
    connect(ui->roomIdLineEdit, &QLineEdit::textChanged, this, &MenuWidget::validateData);
    connect(ui->roomPinLineEdit, &QLineEdit::textChanged, this, &MenuWidget::validateData);
}

void MenuWidget::createRoomBtnHit(){
    emit createRoomRequested();
}

void MenuWidget::roomCreated(const QString& room_id, const QString& room_pin){
    QMessageBox::information(this, "Room created", "Room ID: " + room_id + "\nRoom PIN: " + room_pin);
}

void MenuWidget::joinRoomBtnHit(){
    QString room_id = ui->roomIdLineEdit->text();
    QString room_pin = ui->roomPinLineEdit->text();
    emit joinRoomRequested(room_id, room_pin);
}

void MenuWidget::validateData(){
    if(ui->roomIdLineEdit->text().length() > 0 && ui->roomPinLineEdit->text().length() == 4)
        ui->joinRoomBtn->setEnabled(true);
    else
        ui->joinRoomBtn->setEnabled(false);
}

void MenuWidget::roomError(const QString error){
    QMessageBox::critical(this, "Error", error);
}

