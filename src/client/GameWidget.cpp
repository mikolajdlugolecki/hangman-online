#include "GameWidget.h"

#include "GameState.h"
#include "ui_GameWidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent), ui(new Ui::GameWidget)
{
    ui->setupUi(this);
    ui->guessLineEdit->focusWidget();
    connect(ui->guessButton, &QPushButton::clicked, this, &GameWidget::guessButtonHit);
    connect(ui->guessLineEdit, &QLineEdit::returnPressed, ui->guessButton, &QPushButton::click);
    connect(timer, &QTimer::timeout, this, &GameWidget::updateTime);
}

GameWidget::~GameWidget()
{
    delete ui;
}

void GameWidget::init(const QString wordLength, const QString maxErrors, const QString maxSeconds, const QString coveredWord)
{
    GameState::instance().wordWithHiddenChars = coveredWord;
    ui->maskedWordLabel->setText(transformWord(GameState::instance().wordWithHiddenChars));

    GameState::instance().currentErrors = 0;

    ui->wordLengthLabel->setText(wordLength);
    QString errorsLabel =
        QString::fromStdString(std::to_string(GameState::instance().currentErrors)) + " / " + maxErrors;
    ui->errorsLabel->setText(errorsLabel);

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

QString GameWidget::transformWord(QString word)
{
    QString result = "";
    for (auto &c : word)
    {
        result += c;
        result += " ";
    }
    return result;
}

void GameWidget::guessCorrect(QString newWordWithHiddenChars)
{
    GameState::instance().wordWithHiddenChars = newWordWithHiddenChars;
    ui->maskedWordLabel->setText(transformWord(newWordWithHiddenChars));
}

void GameWidget::guessIncorrect()
{
    QString errorsLabel = QString::fromStdString(std::to_string(++GameState::instance().currentErrors)) + " / " +
                          GameState::instance().maxErrors;
    ui->errorsLabel->setText(errorsLabel);
}

void GameWidget::gameStatsReceived(std::vector<std::string> stats)
{
    ui->listWidget->clear();
    for(auto &stat : stats)
    {
        ui->listWidget->addItem(QString::fromStdString(stat));
    }
}

void GameWidget::updateTime()
{
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    elapsedSeconds--;
}
