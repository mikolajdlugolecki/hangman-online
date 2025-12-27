#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>

namespace Ui{
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    void init(const QString word_length, const QString max_errors);

private:
    Ui::GameWidget* ui;

signals:
};

#endif // GAMEWIDGET_H
