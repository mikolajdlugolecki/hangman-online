#include "GameWidget.h"

#include "GameState.h"
#include "NonBlockingMessagebox.h"
#include "ui_GameWidget.h"

#include <QMessageBox>

GameWidget::GameWidget(QWidget *parent) : QWidget(parent), ui(new Ui::GameWidget)
{
    this->ui->setupUi(this);
    connect(this->timer, &QTimer::timeout, this, &GameWidget::updateTime);

    QFont monoFont("monospace");
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setPointSize(10);
    this->ui->listWidget->setFont(monoFont);
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

    isCharacterUsed.clear();
    for (int i = static_cast<int>('A'); i <= static_cast<int>('Z'); i++)
    {
        isCharacterUsed[static_cast<QChar>(i)] = false;
    }
    regenerateCharacterButtons();

    drawHangman("0");
}

#include <iostream>

void GameWidget::gameRejoined(QString errors, QString score, QString word, QString usedCharacters)
{
    QTimer::singleShot(100,
                       this,
                       [=]()
                       {
                           GameState::instance().currentErrors = errors.toInt();
                           GameState::instance().currentScore = score.toInt();
                           GameState::instance().wordWithHiddenChars = word;

                           this->ui->errorsLabel->setText(errors);
                           this->ui->maskedWordLabel->setText(transformWord(word));
                           this->ui->scoreLabel->setText(score + " / 100");

                           drawHangman(errors);

                           for (QChar c : usedCharacters)
                           {
                               isCharacterUsed[c] = true;
                           }
                           regenerateCharacterButtons();
                       });
}

void GameWidget::regenerateCharacterButtons()
{
    auto gridLayout = this->ui->charactersGridLayout;

    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr)
    {
        gridLayout->removeItem(item);
        if (item->widget())
        {
            item->widget()->deleteLater();
        }
        delete item;
    }

    int maxColumns = 7;

    for (int i = 0; i < 'Z' - 'A' + 1; ++i)
    {
        int row = i / maxColumns;
        int column = i % maxColumns;

        QChar character = static_cast<QChar>('A' + i);

        QPushButton *button = new QPushButton(character, this);
        button->setMinimumSize(40, 40);
        QSizePolicy sp = button->sizePolicy();
        sp.setRetainSizeWhenHidden(true);
        button->setSizePolicy(sp);

        if (isCharacterUsed[character])
        {
            button->hide();
        }

        gridLayout->addWidget(button, row, column);

        connect(button,
                &QPushButton::clicked,
                [=]()
                {
                    QString letter = button->text();
                    isCharacterUsed[letter[0]] = true;
                    button->hide();
                    GameState::instance().lastGuessedLetter = letter;
                    emit this->guessRequested(letter);
                });
    }
}

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

void GameWidget::drawHangman(QString errors)
{
    QString path = QString(":/images/images/hangman_%1.png").arg(errors);

    QPixmap pix(path);
    if (!pix.isNull())
    {
        this->ui->imgLabel->setPixmap(pix);
    }
    else
    {
        qDebug() << "Resource not found at:" << path;
    }
}

void GameWidget::guessIncorrect(const QString &currentScore)
{
    GameState::instance().currentScore = currentScore.toInt();

    QString currentErrors = QString::fromStdString(std::to_string(++GameState::instance().currentErrors));
    QString errorsLabel = currentErrors + " / " + GameState::instance().maxErrors;
    this->ui->errorsLabel->setText(errorsLabel);
    this->ui->scoreLabel->setText(currentScore + " / 100");

    drawHangman(currentErrors);
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
    auto gridLayout = this->ui->charactersGridLayout;
    for (int i = 0; i < gridLayout->count(); ++i)
    {
        QWidget *widget = gridLayout->itemAt(i)->widget();
        if (QPushButton *btn = qobject_cast<QPushButton *>(widget))
        {
            btn->setEnabled(false);
        }
    }

    switch (type)
    {
    case 1:
    {
        auto *box = new NonBlockingMessageBox(
            this,
            "Info",
            "Congratulations!\n\nYour results:\n\nPoints: " + QString::fromStdString(payload[0]) +
                "\nErrors: " + QString::fromStdString(payload[1]));
        box->showWithTimeout();
    }
    break;
    case 2:
    {
        auto *box = new NonBlockingMessageBox(
            this,
            "Info",
            "Maximum errors reached\n\nYour results:\n\nPoints: " + QString::fromStdString(payload[0]) +
                "\nErrors: " + QString::fromStdString(payload[1]));
        box->showWithTimeout();
    }
    break;
    case 3:
    {
        auto *box = new NonBlockingMessageBox(
            this,
            "Info",
            "All players already finished\n\nFinal result:\n\n" + QString::fromStdString(payload[0]) +
                QString::fromStdString(payload[1]) + (payload.size() == 3 ? QString::fromStdString(payload[2]) : ""));
        box->showWithTimeout();
        connect(box, &QMessageBox::buttonClicked, this, [this]() { emit this->leaveGame(); });
        connect(box, &NonBlockingMessageBox::timedOut, this, [this]() { emit this->leaveGame(); });
    }
    break;
    case 4:
    {
        auto *box = new NonBlockingMessageBox(this,
                                              "Info",
                                              "Time's up!\n\nFinal result:\n\n" + QString::fromStdString(payload[0]) +
                                                  QString::fromStdString(payload[1]) +
                                                  (payload.size() == 3 ? QString::fromStdString(payload[2]) : ""));
        box->showWithTimeout();
        connect(box, &QMessageBox::buttonClicked, this, [this]() { emit this->leaveGame(); });
        connect(box, &NonBlockingMessageBox::timedOut, this, [this]() { emit this->leaveGame(); });
    }
    break;
    }
}

void GameWidget::updateTime()
{
    const int minutes = GameState::instance().remainingTime / 60;
    const int seconds = GameState::instance().remainingTime % 60;

    this->ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}
