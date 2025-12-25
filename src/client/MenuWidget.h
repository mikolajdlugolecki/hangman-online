#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>

namespace Ui {
class MenuWidget;
}

class MenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MenuWidget(QWidget *parent = nullptr);
    void roomCreated(const QString& room_id, const QString& room_pin);
    void roomError(const QString error);

private:
    Ui::MenuWidget *ui;
    void createRoomBtnHit();
    void joinRoomBtnHit();
    void validateData();

signals:
    void createRoomRequested();
    void joinRoomRequested(const QString& room_id, const QString& room_pin);
};

#endif // MENUWIDGET_H
