#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConnectWidget.h"
#include "GameWidget.h"
#include "LobbyWidget.h"
#include "LoginWidget.h"
#include "MenuWidget.h"
#include "TcpClient.h"

#include <QMainWindow>
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *stack;

    TcpClient *client;

    ConnectWidget *ConnectScene;
    LoginWidget *LoginScene;
    MenuWidget *MenuScene;
    LobbyWidget *LobbyScene;
    GameWidget *GameScene;
};

#endif // MAINWINDOW_H
