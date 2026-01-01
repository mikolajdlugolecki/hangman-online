#include "ConnectWidget.h"

#include "NonBlockingMessagebox.h"
#include "ui_ConnectWidget.h"

#include <QMessageBox>

ConnectWidget::ConnectWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectWidget)
{
    this->ui->setupUi(this);
    connect(this->ui->connectBtn, &QPushButton::clicked, this, &ConnectWidget::connectButtonHit);
    connect(this->ui->hostLineEdit, &QLineEdit::returnPressed, this->ui->connectBtn, &QPushButton::click);
}

ConnectWidget::~ConnectWidget()
{
    delete this->ui;
}

void ConnectWidget::connectButtonHit()
{
    this->ui->hostLineEdit->setEnabled(false);
    this->ui->portSpinBox->setEnabled(false);
    this->ui->connectBtn->setEnabled(false);
    const QString host = this->ui->hostLineEdit->text();
    const quint16 port = this->ui->portSpinBox->value();
    emit this->connectionRequested(host, port);
}

void ConnectWidget::connectionTimedOut()
{
    auto *box = new NonBlockingMessageBox(this, "Error", "Connection timed out", QMessageBox::Warning);
    box->showWithTimeout();
    this->ui->hostLineEdit->setEnabled(true);
    this->ui->portSpinBox->setEnabled(true);
    this->ui->connectBtn->setEnabled(true);
}

void ConnectWidget::connectionLost()
{
    auto *box = new NonBlockingMessageBox(this, "Error", "Connection lost", QMessageBox::Critical);
    box->showWithTimeout();
    this->ui->hostLineEdit->setEnabled(true);
    this->ui->portSpinBox->setEnabled(true);
    this->ui->connectBtn->setEnabled(true);
}
