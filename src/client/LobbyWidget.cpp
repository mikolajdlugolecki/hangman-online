#include "LobbyWidget.h"
#include "ui_LobbyWidget.h"

LobbyWidget::LobbyWidget(QWidget *parent)
    : QWidget{parent}, ui(new Ui::LobbyWidget)
{
    ui->setupUi(this);
}
