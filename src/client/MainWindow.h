#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "TcpClient.h"
#include "ConnectWidget.h"
#include "LoginWidget.h"
#include "MenuWidget.h"
#include "LobbyWidget.h"
#include "GameWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget* stack;

    TcpClient* client;

    ConnectWidget* ConnectScene;
    LoginWidget* LoginScene;
    MenuWidget* MenuScene;
    LobbyWidget* LobbyScene;
    GameWidget* GameScene;
};

#endif // MAINWINDOW_H
