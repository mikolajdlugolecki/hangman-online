#include "GameWidget.h"

#include "GameState.h"
#include "ui_GameWidget.h"

#include <QMessageBox>

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
    ui->guessButton->setEnabled(true);
    ui->guessLineEdit->setEnabled(true);

    GameState::instance().wordWithHiddenChars = coveredWord;
    GameState::instance().inProgress = true;
    this->ui->maskedWordLabel->setText(transformWord(GameState::instance().wordWithHiddenChars));

    GameState::instance().currentErrors = 0;
    GameState::instance().currentScore = 0;

    this->ui->wordLengthLabel->setText(wordLength);
    QString errorsLabel =
        QString::fromStdString(std::to_string(GameState::instance().currentErrors)) + " / " + maxErrors;
    this->ui->errorsLabel->setText(errorsLabel);

    QString scoreLabel = QString::fromStdString(std::to_string(GameState::instance().currentScore)) + " / 100";
    this->ui->scoreLabel->setText(scoreLabel);

    GameState::instance().remainingTime = maxSeconds.toInt();
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

void GameWidget::guessCorrect(const QString &newWordWithHiddenChars, const QString &currentScore)
{
    GameState::instance().wordWithHiddenChars = newWordWithHiddenChars;
    GameState::instance().currentScore = currentScore.toInt();

    this->ui->maskedWordLabel->setText(transformWord(newWordWithHiddenChars));    
    this->ui->scoreLabel->setText(currentScore + " / 100");
}

void GameWidget::guessIncorrect(const QString &currentScore)
{
    GameState::instance().currentScore = currentScore.toInt();

    QString errorsLabel = QString::fromStdString(std::to_string(++GameState::instance().currentErrors)) + " / " +
                          GameState::instance().maxErrors;
    this->ui->errorsLabel->setText(errorsLabel);
    this->ui->scoreLabel->setText(currentScore + " / 100");
}

void GameWidget::gameStatsReceived(const std::vector<std::string> &stats)
{
    this->ui->listWidget->clear();
    for (auto &stat : stats)
    {
        this->ui->listWidget->addItem(QString::fromStdString(stat));
    }
}

void GameWidget::gameRemainingTimeReceived(const QString &seconds)
{
    GameState::instance().remainingTime = seconds.toInt();
    this->updateTime();
}

void GameWidget::roundOverReceived(int type, const std::vector<std::string> &payload)
{
    this->ui->guessButton->setEnabled(false);
    this->ui->guessLineEdit->setEnabled(false);

    switch (type)
    {
    case 1:
        QMessageBox::information(this,
                                 "Congratulations!",
                                 "Your results:\n\nPoints: " + QString::fromStdString(payload[0]) +
                                     "\nErrors: " + QString::fromStdString(payload[1]));
        break;
    case 2:
        QMessageBox::critical(this, "Info", "Maximum errors reached\n\nYour results:\n\nPoints: " + QString::fromStdString(payload[0]) + "\nErrors: " + QString::fromStdString(payload[1]));
        break;
    case 3:
        QMessageBox::information(this,
                                 "Everyone's done",
                                 "Final result:\n\n" + QString::fromStdString(payload[0]) +
                                     QString::fromStdString(payload[1]) +
                                     (payload.size() == 3 ? QString::fromStdString(payload[2]) : ""));
        emit leaveGame();
        break;
    case 4:
        QMessageBox::critical(this,
                              "Time's up!",
                              "Final result:\n\n" + QString::fromStdString(payload[0]) +
                                  QString::fromStdString(payload[1]) +
                                  (payload.size() == 3 ? QString::fromStdString(payload[2]) : ""));
        emit leaveGame();
        break;
    }
}

void GameWidget::updateTime()
{
    const int minutes = GameState::instance().remainingTime / 60;
    const int seconds = GameState::instance().remainingTime % 60;

    this->ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}
