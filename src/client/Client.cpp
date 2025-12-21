#include "Client.h"
#include "ui_Client.h"

#include <QMessageBox>

Client::Client(QWidget *parent) : QWidget(parent), ui(new Ui::Client) {
    ui->setupUi(this);
    connTimeoutTimer->setSingleShot(true);
    connect(ui->conectBtn, &QPushButton::clicked, this, &Client::connectBtnHit);
    connect(ui->hostLineEdit, &QLineEdit::returnPressed, ui->conectBtn, &QPushButton::click);
    connect(ui->sendBtn, &QPushButton::clicked, this, &Client::sendBtnHit);
    connect(ui->msgLineEdit, &QLineEdit::returnPressed, ui->sendBtn, &QPushButton::click);
}

Client::~Client() {
    socket->close();
    delete ui;
}

void Client::connectBtnHit() {
    ui->connectGroup->setEnabled(false);
    ui->msgsTextEdit->append("<b>Connecting to " + ui->hostLineEdit->text() + ":" + QString::number(ui->portSpinBox->value()) + "...</b>");
    connect(socket, &QTcpSocket::connected, this, &Client::socketConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Client::socketDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &Client::socketError);
    connect(socket, &QTcpSocket::readyRead, this, &Client::socketRead);
    connect(connTimeoutTimer, &QTimer::timeout, this, &Client::connTimedOut);
    socket->connectToHost(ui->hostLineEdit->text(), ui->portSpinBox->value());
    connTimeoutTimer->start(3000);
}

void Client::connTimedOut() {
    socket->disconnectFromHost();
    connTimeoutTimer->disconnect();
    ui->connectGroup->setEnabled(true);
    ui->msgsTextEdit->append("<b>Connection timed out</b>");
    QMessageBox::critical(this, "Error", "Connection timed out");
}

void Client::socketConnected() {
    connTimeoutTimer->stop();
    connTimeoutTimer->disconnect();
    ui->msgsTextEdit->append("<b>Connected</b>");
    ui->talkGroup->setEnabled(true);
}

void Client::socketDisconnected() {

}

void Client::socketError() {

}

void Client::socketRead() {
    this->buffer.append(socket->readAll());
    while(true){
        switch(this->readState){
        case ReadState::READ_TYPE:{
            if (this->buffer.size() < 1) return;
            uint8_t type = static_cast<uint8_t>(this->buffer[0]);
            this->buffer.remove(0, 1);
            switch(type){
            case Response::GAME_STARTED:
                break;
            case Response::GAME_STATE:
                break;
            case Response::GAME_SUMMARY:
                break;
            case Response::PING:
                break;
            case Response::REAUTH:
                break;
            case Response::ROOM_CREATED:
                break;
            default:
                break;
            }
            this->readState = ReadState::READ_LENGTH;
            break;}
        case ReadState::READ_LENGTH:{
            if (this->buffer.size() < 4) return;
            uint32_t length;
            memcpy(&length, this->buffer.constData(), 4);
            length = ntohl(length);
            this->buffer.remove(0, 4);
            this->incomingMessageLength = length;
            this->readState = ReadState::READ_PAYLOAD;
            break;}
        case ReadState::READ_PAYLOAD:{
            if (this->buffer.size() < static_cast<int>(this->incomingMessageLength)) return;
            QByteArray payload = this->buffer.left(this->incomingMessageLength);
            this->buffer.remove(0, this->incomingMessageLength);
            ui->msgsTextEdit->append(QString::fromUtf8(payload).trimmed());
            this->incomingMessageLength = 0;
            this->readState = ReadState::READ_TYPE;
            break;}
        }
    }
    ui->msgsTextEdit->setAlignment(Qt::AlignLeft);
}

void Client::sendBtnHit() {
    QString txt = ui->msgLineEdit->text().trimmed();
    QByteArray txtAsUtf8 = txt.toUtf8();
    this->construct_request(Request::CREATE_ROOM, txtAsUtf8);
    ui->msgsTextEdit->append(txt);
    ui->msgsTextEdit->setAlignment(Qt::AlignRight);
    ui->msgLineEdit->clear();
    ui->msgLineEdit->setFocus();
}

int Client::construct_request(const Request::Type type, const QByteArray payload) {
    QByteArray packet;
    uint8_t raw_type = static_cast<uint8_t>(type);
    packet.append(static_cast<char>(raw_type));
    uint32_t raw_size = htonl(static_cast<uint32_t>(payload.size()));
    packet.append(reinterpret_cast<const char*>(&raw_size), 4);
    packet.append(payload);
    socket->write(packet);
    return 0;
}
