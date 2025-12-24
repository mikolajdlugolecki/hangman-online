#include "ConnectWidget.h"
#include "ui_ConnectWidget.h"

#include <QMessageBox>

ConnectWidget::ConnectWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectWidget) {
    ui->setupUi(this);
    connect(ui->connectBtn, &QPushButton::clicked, this, &ConnectWidget::connectBtnHit);
    connect(ui->hostLineEdit, &QLineEdit::returnPressed, ui->connectBtn, &QPushButton::click);
}

ConnectWidget::~ConnectWidget() {

}

void ConnectWidget::connectBtnHit() {
    ui->hostLineEdit->setEnabled(false);
    ui->portSpinBox->setEnabled(false);
    ui->connectBtn->setEnabled(false);
    QString host = ui->hostLineEdit->text();
    quint16 port = ui->portSpinBox->value();
    emit connectionRequested(host, port);
}

void ConnectWidget::connTimedOut() {
    QMessageBox::critical(this, "Error", "Connection timed out");
    ui->hostLineEdit->setEnabled(true);
    ui->portSpinBox->setEnabled(true);
    ui->connectBtn->setEnabled(true);
}

// void ConnectWidget::socketRead() {
//     this->buffer.append(socket->readAll());
//     auto *msg = new Message();
//     std::vector<char> vec(this->buffer.toStdString().begin(), this->buffer.toStdString().end());
//     this->parser->parse(vec, msg);
// }
