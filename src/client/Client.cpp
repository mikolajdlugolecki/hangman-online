#include "Client.h"
#include "ui_Client.h"
#include "../common/Message.h"
#include "../common/MessageType.h"
#include "../common/Serializer.h"

#include <QMessageBox>

Client::Client(QWidget *parent) : QWidget(parent), ui(new Ui::Client) {
    ui->setupUi(this);
    connTimeoutTimer->setSingleShot(true);
    connect(ui->conectBtn, &QPushButton::clicked, this, &Client::connectBtnHit);
    connect(ui->hostLineEdit, &QLineEdit::returnPressed, ui->conectBtn, &QPushButton::click);
    connect(ui->sendBtn, &QPushButton::clicked, this, &Client::sendBtnHit);
    connect(ui->msgLineEdit, &QLineEdit::returnPressed, ui->sendBtn, &QPushButton::click);
    this->parser = new Parser();
}

Client::~Client() {
    socket->close();
    delete ui;
    delete parser;
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
    auto *msg = new Message();
    std::vector<char> vec(this->buffer.toStdString().begin(), this->buffer.toStdString().end());
    this->parser->parse(vec, msg);
    ui->msgsTextEdit->append(QString::fromUtf8(msg->payload).trimmed());
    ui->msgsTextEdit->setAlignment(Qt::AlignLeft);
}

void Client::sendBtnHit() {
    QString txt = ui->msgLineEdit->text().trimmed();
    QByteArray txtAsUtf8 = txt.toUtf8();
    auto *msg = new Message();
    msg->type = Request::CREATE_ROOM;
    msg->length = txtAsUtf8.size();
    msg->payload = txtAsUtf8.toStdString();
    std::vector<char> tmp = Serializer::serialize(*msg);
    QByteArray ba(tmp.data(), static_cast<int>(tmp.size()));
    socket->write(ba);
    ui->msgsTextEdit->append(txt);
    ui->msgsTextEdit->setAlignment(Qt::AlignRight);
    ui->msgLineEdit->clear();
    ui->msgLineEdit->setFocus();
}
