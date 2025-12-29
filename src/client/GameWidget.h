#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QTimer>
#include <QWidget>

#include <QPushButton>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QAbstractItemView>

namespace Ui
{
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    void init(const QString wordLength, const QString maxErrors, const QString maxSeconds, const QString coveredWord);
    void guessCorrect(QString newWordWithHiddenChars);
    void guessIncorrect();
    void gameStatsReceived(std::vector<std::string> stats);

private:
    Ui::GameWidget *ui;
    QTimer *timer = new QTimer(this);
    int elapsedSeconds = 0;
    void guessButtonHit();
    QString transformWord(QString word);
    void updateTime();

signals:
    void guessRequested(const QString &letter);
};

#endif // GAMEWIDGET_H
