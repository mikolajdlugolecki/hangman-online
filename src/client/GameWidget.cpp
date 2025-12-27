#include "GameWidget.h"
#include "ui_GameWidget.h"
#include "GameState.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget{parent}, ui(new Ui::GameWidget)
{
    ui->setupUi(this);
}

void GameWidget::init(const QString word_length, const QString max_errors){
    GameState::instance().word = "";
    GameState::instance().current_errors = 0;
    ui->wordLengthLabel->setText(word_length);
    QString errorsLabel = QString::fromStdString(std::to_string(GameState::instance().current_errors)) + " / " + max_errors;
    ui->errorsLabel->setText(errorsLabel);
    for(int i = 0; i < GameState::instance().word_length.toInt(); i++){
        GameState::instance().word += "_ ";
    }
    ui->maskedWordLabel->setText(GameState::instance().word);
}
