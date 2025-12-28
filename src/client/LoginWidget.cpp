#include "LoginWidget.h"
#include "ui_LoginWidget.h"

#include <QMessageBox>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    connect(ui->loginBtn, &QPushButton::clicked, this, &LoginWidget::loginButtonHit);
    connect(ui->loginLineEdit, &QLineEdit::returnPressed, ui->loginBtn, &QPushButton::click);
}

void LoginWidget::loginButtonHit()
{
    QString nickname = ui->loginLineEdit->text();
    if(nickname.size() < 3)
    {
        QMessageBox::critical(this, "Error", "Nickname must be at least 3 characters long");
        return;
    }

    GameState::instance().usersNickname = nickname;
    emit nicknameSent(nickname);
}

void LoginWidget::nicknameError(const QString error)
{
    QMessageBox::critical(this, "Error", error);
}
