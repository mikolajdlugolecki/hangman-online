#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QAbstractItemView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QTimer>
#include <QWidget>

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
    void guessCorrect(const QString &newWordWithHiddenChars);
    void guessIncorrect();
    void gameStatsReceived(const std::vector<std::string> &stats);

private:
    Ui::GameWidget *ui;
    QTimer *timer = new QTimer(this);
    int elapsedSeconds = 0;
    void guessButtonHit();
    QString transformWord(const QString &word);
    void updateTime();

signals:
    void guessRequested(const QString &letter);
};

#endif // GAMEWIDGET_H
