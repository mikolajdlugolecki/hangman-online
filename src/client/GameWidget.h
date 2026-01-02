#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QAbstractItemView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>

namespace Ui
{
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget() override;
    void init(const QString &wordLength,
              const QString &maxErrors,
              const QString &maxSeconds,
              const QString &coveredWord);
    void guessCorrect(const QString &newWordWithHiddenChars, const QString &currentScore);
    void guessIncorrect(const QString &currentScore);
    void gameStatsReceived(const std::vector<std::string> &stats);
    void gameRemainingTimeReceived(const QString &seconds);
    void roundOverReceived(int type, const std::vector<std::string> &payload);

private:
    Ui::GameWidget *ui;
    QTimer *timer = new QTimer(this);
    QVector<QString> unusedCharacters;

    void regenerateCharacterButtons();
    QString transformWord(const QString &word);
    void updateTime();
    void drawHangman(QString errors);

signals:
    void guessRequested(const QString &letter);
    void leaveGame();
};

#endif // GAMEWIDGET_H
