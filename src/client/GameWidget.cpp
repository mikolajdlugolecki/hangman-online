#include "GameWidget.h"

#include "GameState.h"
#include "ui_GameWidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent), ui(new Ui::GameWidget)
{
    this->ui->setupUi(this);
    this->ui->guessLineEdit->focusWidget();
    connect(this->ui->guessButton, &QPushButton::clicked, this, &GameWidget::guessButtonHit);
    connect(this->ui->guessLineEdit, &QLineEdit::returnPressed, this->ui->guessButton, &QPushButton::click);
    connect(this->timer, &QTimer::timeout, this, &GameWidget::updateTime);
}

GameWidget::~GameWidget()
{
    delete this->ui;
}

void GameWidget::init(const QString &wordLength,
                      const QString &maxErrors,
                      const QString &maxSeconds,
                      const QString &coveredWord)
{
    GameState::instance().wordWithHiddenChars = coveredWord;
    this->ui->maskedWordLabel->setText(transformWord(GameState::instance().wordWithHiddenChars));

    GameState::instance().currentErrors = 0;

    this->ui->wordLengthLabel->setText(wordLength);
    QString errorsLabel =
        QString::fromStdString(std::to_string(GameState::instance().currentErrors)) + " / " + maxErrors;
    this->ui->errorsLabel->setText(errorsLabel);

    this->elapsedSeconds = maxSeconds.toInt();
    this->timer->start(1000);
    this->updateTime();
}

void GameWidget::guessButtonHit()
{
    const QString letter = this->ui->guessLineEdit->text();
    if (letter.length() < 1)
    {
        return;
    }
    this->ui->guessLineEdit->clear();
    GameState::instance().lastGuessedLetter = letter;
    emit this->guessRequested(letter);
};

QString GameWidget::transformWord(const QString &word)
{
    QString result = "";
    for (auto &c : word)
    {
        result += c;
        result += " ";
    }
    return result;
}

void GameWidget::guessCorrect(const QString &newWordWithHiddenChars)
{
    GameState::instance().wordWithHiddenChars = newWordWithHiddenChars;
    this->ui->maskedWordLabel->setText(transformWord(newWordWithHiddenChars));
}

void GameWidget::guessIncorrect()
{
    QString errorsLabel = QString::fromStdString(std::to_string(++GameState::instance().currentErrors)) + " / " +
                          GameState::instance().maxErrors;
    this->ui->errorsLabel->setText(errorsLabel);
}

void GameWidget::gameStatsReceived(const std::vector<std::string> &stats)
{
    this->ui->listWidget->clear();
    for (auto &stat : stats)
    {
        this->ui->listWidget->addItem(QString::fromStdString(stat));
    }
}

void GameWidget::updateTime()
{
    const int minutes = elapsedSeconds / 60;
    const int seconds = elapsedSeconds % 60;

    this->ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    this->elapsedSeconds--;
}
