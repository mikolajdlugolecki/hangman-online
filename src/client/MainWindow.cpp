#include "MainWindow.h"
#include "GameState.h"

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
    LobbyScene = new LobbyWidget(this);

    stack->addWidget(ConnectScene);
    stack->addWidget(LoginScene);
    stack->addWidget(MenuScene);
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
        stack->setCurrentWidget(MenuScene);
    });

    connect(MenuScene, &MenuWidget::createRoomRequested, client, &TcpClient::createRoomReceived);
    connect(MenuScene, &MenuWidget::joinRoomRequested, client, &TcpClient::joinRoomReceived);
    connect(client, &TcpClient::roomError, MenuScene, &MenuWidget::roomError);

    connect(client, &TcpClient::roomCreated, this, [this](const QString roomId, const QString roomPin) {
        GameState::instance().roomId = roomId;
        GameState::instance().roomPin = roomPin;
        GameState::instance().isRoomOwner = true;
        GameState::instance().roomPlayers.push_back(GameState::instance().usersNickname);

        stack->setCurrentWidget(LobbyScene);
    });

    connect(client, &TcpClient::roomOK, this, [this](){
        stack->setCurrentWidget(LobbyScene);
    });

    connect(client, &TcpClient::updateLobbyPlayerList, LobbyScene, &LobbyWidget::updateLobbyPlayerList);
}
