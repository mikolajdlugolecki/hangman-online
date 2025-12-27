#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "GameState.h"

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
    void loginBtnHit();
    void nicknameError(const QString error);

private:
    Ui::LoginWidget *ui;

signals:
    void loginSuccess();
    void nicknameSent(const QString& nickname);
};

#endif // LOGINWIDGET_H
