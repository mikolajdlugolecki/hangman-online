#ifndef NONBLOCKINGMESSAGEBOX_H
#define NONBLOCKINGMESSAGEBOX_H

#include <QMessageBox>
#include <QObject>
#include <QTimer>

class NonBlockingMessageBox : public QMessageBox
{
    Q_OBJECT
public:
    NonBlockingMessageBox(QWidget *parent = nullptr,
                          const QString title = "Info",
                          const QString content = "Placeholder",
                          const QMessageBox::Icon icon = QMessageBox::NoIcon);
    void showWithTimeout(const int timeoutInSeconds = 5);

private:
    QTimer *timer = new QTimer(this);
    int remainingSeconds;

signals:
    void timedOut();
};

#endif // NONBLOCKINGMESSAGEBOX_H
