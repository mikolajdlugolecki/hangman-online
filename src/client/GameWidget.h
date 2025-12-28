#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

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
    void init(const QString wordLength, const QString maxErrors, const QString maxSeconds);
    void guessPositions(std::vector<std::string> positions);
    void guessIncorrect();
    void gameStatsReceived(std::vector<std::vector<std::string>> stats);

private:
    Ui::GameWidget* ui;
    QTimer* timer = new QTimer(this);
    int elapsedSeconds = 0;
    void guessButtonHit();
    QString transformWord(std::vector<char> word);
    void updateTime();

signals:
    void guessRequested(const QString& letter);
};

#endif // GAMEWIDGET_H
