#include "MenuWidget.h"

#include "NonBlockingMessagebox.h"
#include "ui_MenuWidget.h"

#include <QMessageBox>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MenuWidget)
{
    this->ui->setupUi(this);
    connect(this->ui->createRoomBtn, &QPushButton::clicked, this, &MenuWidget::createRoomButtonHit);
    connect(this->ui->joinRoomBtn, &QPushButton::clicked, this, &MenuWidget::joinRoomButtonHit);
    connect(this->ui->roomPinLineEdit, &QLineEdit::returnPressed, this->ui->joinRoomBtn, &QPushButton::click);
    connect(this->ui->roomIdLineEdit, &QLineEdit::textChanged, this, &MenuWidget::validateData);
    connect(this->ui->roomPinLineEdit, &QLineEdit::textChanged, this, &MenuWidget::validateData);

    QRegularExpression rx("^[0-9_]{0,4}$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->roomIdLineEdit->setValidator(validator);
    ui->roomPinLineEdit->setValidator(validator);
}

MenuWidget::~MenuWidget()
{
    delete this->ui;
}

void MenuWidget::createRoomButtonHit()
{
    emit this->createRoomRequested();
}

void MenuWidget::joinRoomButtonHit()
{
    const QString roomId = this->ui->roomIdLineEdit->text();
    const QString roomPin = this->ui->roomPinLineEdit->text();

    GameState::instance().roomId = roomId;
    GameState::instance().roomPin = roomPin;
    GameState::instance().isRoomOwner = false;

    emit this->joinRoomRequested(roomId, roomPin);
}

void MenuWidget::validateData()
{
    if (this->ui->roomIdLineEdit->text().length() > 0 && this->ui->roomPinLineEdit->text().length() == 4)
    {
        this->ui->joinRoomBtn->setEnabled(true);
    }
    else
    {
        this->ui->joinRoomBtn->setEnabled(false);
    }
}

void MenuWidget::roomError(const QString &error)
{
    auto *box = new NonBlockingMessageBox(this, "Error", error, QMessageBox::Warning);
    box->showWithTimeout();
}
