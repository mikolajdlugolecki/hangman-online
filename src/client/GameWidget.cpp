#include "GameWidget.h"

#include "GameState.h"
#include "ui_GameWidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent), ui(new Ui::GameWidget)
{
    ui->setupUi(this);
    ui->guessLineEdit->focusWidget();
    ui->statsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->guessButton, &QPushButton::clicked, this, &GameWidget::guessButtonHit);
    connect(ui->guessLineEdit, &QLineEdit::returnPressed, ui->guessButton, &QPushButton::click);
    connect(timer, &QTimer::timeout, this, &GameWidget::updateTime);
}

void GameWidget::init(const QString wordLength, const QString maxErrors, const QString maxSeconds)
{
    GameState::instance().word.clear();
    GameState::instance().currentErrors = 0;
    ui->wordLengthLabel->setText(wordLength);
    QString errorsLabel =
        QString::fromStdString(std::to_string(GameState::instance().currentErrors)) + " / " + maxErrors;
    ui->errorsLabel->setText(errorsLabel);
    for (int i = 0; i < GameState::instance().wordLength.toInt(); i++)
    {
        GameState::instance().word.push_back('_');
    }
    ui->maskedWordLabel->setText(transformWord(GameState::instance().word));
    elapsedSeconds = maxSeconds.toInt();
    timer->start(1000);
    updateTime();
}

void GameWidget::guessButtonHit()
{
    QString letter = ui->guessLineEdit->text();
    if (letter.length() < 1)
        return;
    ui->guessLineEdit->clear();
    GameState::instance().lastGuessedLetter = letter;
    emit guessRequested(letter);
};

QString GameWidget::transformWord(std::vector<char> word)
{
    QString result = "";
    for (auto &c : word)
    {
        result += c;
        result += " ";
    }
    return result;
}

void GameWidget::guessPositions(std::vector<std::string> positions)
{
    for (auto &pos : positions)
    {
        GameState::instance().word[std::stoi(pos)] =
            std::toupper(GameState::instance().lastGuessedLetter.toStdString()[0]);
    }
    ui->maskedWordLabel->setText(transformWord(GameState::instance().word));
}

void GameWidget::guessIncorrect()
{
    QString errorsLabel = QString::fromStdString(std::to_string(++GameState::instance().currentErrors)) + " / " +
                          GameState::instance().maxErrors;
    ui->errorsLabel->setText(errorsLabel);
}

void GameWidget::gameStatsReceived(std::vector<std::vector<std::string>> stats)
{
    for (std::vector<std::string> dataRow : stats)
    {
        int row = ui->statsTable->rowCount();
        ui->statsTable->insertRow(row);
        for (size_t i = 0; dataRow.size(); i++)
        {
            ui->statsTable->setItem(row, i, new QTableWidgetItem(QString::fromStdString(dataRow[i])));
        }
    }
    ui->statsTable->resizeColumnsToContents();
}

void GameWidget::updateTime()
{
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    elapsedSeconds--;
}
