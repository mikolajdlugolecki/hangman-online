#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    this->setWindowTitle("Hangman");
    this->resize(900, 600);

    client = new TcpClient(this);

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    ConnectScene = new ConnectWidget(this);
    LoginScene = new LoginWidget(this);
    MenuScene = new MenuWidget(this);

    stack->addWidget(ConnectScene);
    stack->addWidget(LoginScene);
    stack->addWidget(MenuScene);

    stack->setCurrentWidget(ConnectScene);

    connect(ConnectScene, &ConnectWidget::connectionRequested, client, &TcpClient::connectToServer);
    connect(client, &TcpClient::timeout, ConnectScene, &ConnectWidget::connTimedOut);

    connect(client, &TcpClient::connected, this, [this]() {
        stack->setCurrentWidget(LoginScene);
    });

    connect(LoginScene, &LoginWidget::nicknameSent, client, &TcpClient::nicknameReceived);
    connect(client, &TcpClient::nicknameError, LoginScene, &LoginWidget::nicknameError);

    connect(client, &TcpClient::nicknameOK, this, [this]() {
        stack->setCurrentWidget(MenuScene);
    });

    connect(MenuScene, &MenuWidget::createRoomRequested, client, &TcpClient::createRoomReceived);
    connect(client, &TcpClient::roomCreated, MenuScene, &MenuWidget::roomCreated);
    connect(MenuScene, &MenuWidget::joinRoomRequested, client, &TcpClient::joinRoomReceived);
    connect(client, &TcpClient::roomError, MenuScene, &MenuWidget::roomError);

    connect(client, &TcpClient::roomOK, this, [this](){
        // go to next screen
    });

    // handle roomCreated signal: capture OK button clicked in pop up and then redirect to new screen if possible, otherwise delete pop up and redirect immediately
}
