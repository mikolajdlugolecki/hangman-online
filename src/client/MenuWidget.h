#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include "GameState.h"

#include <QWidget>

namespace Ui
{
class MenuWidget;
}

class MenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget() override;
    void roomCreated(const QString &roomId, const QString &roomPin);
    void roomError(const QString &error);

private:
    Ui::MenuWidget *ui;
    void createRoomButtonHit();
    void joinRoomButtonHit();
    void validateData();

signals:
    void createRoomRequested();
    void joinRoomRequested(const QString &roomId, const QString &roomPin);
};

#endif // MENUWIDGET_H
