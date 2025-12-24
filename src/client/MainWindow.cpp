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
    LobbyScene = new LobbyWidget(this);

    stack->addWidget(ConnectScene);
    stack->addWidget(LoginScene);
    stack->addWidget(LobbyScene);

    stack->setCurrentWidget(ConnectScene);

    connect(ConnectScene, &ConnectWidget::connectionRequested, client, &TcpClient::connectToServer);
    connect(client, &TcpClient::timeout, ConnectScene, &ConnectWidget::connTimedOut);

    connect(client, &TcpClient::connected, this, [this]() {
        stack->setCurrentWidget(LoginScene);
    });

    connect(LoginScene, &LoginWidget::nicknameSent, client, &TcpClient::nicknameReceived);
    connect(client, &TcpClient::nicknameError, LoginScene, &LoginWidget::nicknameError);

    connect(client, &TcpClient::nicknameOK, this, [this]() {
        stack->setCurrentWidget(LobbyScene);
    });
}
