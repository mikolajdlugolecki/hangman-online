#include "ConnectWidget.h"
#include "ui_ConnectWidget.h"

#include <QMessageBox>

ConnectWidget::ConnectWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectWidget)
{
    ui->setupUi(this);
    connect(ui->connectBtn, &QPushButton::clicked, this, &ConnectWidget::connectButtonHit);
    connect(ui->hostLineEdit, &QLineEdit::returnPressed, ui->connectBtn, &QPushButton::click);
}

ConnectWidget::~ConnectWidget() {}

void ConnectWidget::connectButtonHit()
{
    ui->hostLineEdit->setEnabled(false);
    ui->portSpinBox->setEnabled(false);
    ui->connectBtn->setEnabled(false);
    QString host = ui->hostLineEdit->text();
    quint16 port = ui->portSpinBox->value();
    emit connectionRequested(host, port);
}

void ConnectWidget::connectionTimedOut()
{
    QMessageBox::critical(this, "Error", "Connection timed out");
    ui->hostLineEdit->setEnabled(true);
    ui->portSpinBox->setEnabled(true);
    ui->connectBtn->setEnabled(true);
}

void ConnectWidget::connectionLost()
{
    QMessageBox::critical(this, "Error", "Connection lost");
    ui->hostLineEdit->setEnabled(true);
    ui->portSpinBox->setEnabled(true);
    ui->connectBtn->setEnabled(true);
}
