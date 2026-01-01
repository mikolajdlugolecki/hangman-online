#include "LoginWidget.h"

#include "NonBlockingMessagebox.h"
#include "ui_LoginWidget.h"

#include <QMessageBox>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LoginWidget)
{
    this->ui->setupUi(this);
    connect(this->ui->loginBtn, &QPushButton::clicked, this, &LoginWidget::loginButtonHit);
    connect(this->ui->loginLineEdit, &QLineEdit::returnPressed, this->ui->loginBtn, &QPushButton::click);
}

LoginWidget::~LoginWidget()
{
    delete this->ui;
}

void LoginWidget::loginButtonHit()
{
    const QString nickname = this->ui->loginLineEdit->text();
    if (nickname.size() < 3)
    {
        auto *box = new NonBlockingMessageBox(
            this, "Error", "Nickname must be at least 3 characters long", QMessageBox::Warning);
        box->showWithTimeout();
        return;
    }

    GameState::instance().usersNickname = nickname;
    emit this->nicknameSent(nickname);
}

void LoginWidget::nicknameError(const QString &error)
{
    auto *box = new NonBlockingMessageBox(this, "Error", error, QMessageBox::Critical);
    box->showWithTimeout();
}
