#include "LoginWidget.h"

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
        QMessageBox::critical(this, "Error", "Nickname must be at least 3 characters long");
        return;
    }

    GameState::instance().usersNickname = nickname;
    emit this->nicknameSent(nickname);
}

void LoginWidget::nicknameError(const QString &error)
{
    QMessageBox::critical(this, "Error", error);
}
