#include "NonBlockingMessagebox.h"

NonBlockingMessageBox::NonBlockingMessageBox(QWidget *parent,
                                             const QString title,
                                             const QString content,
                                             const QMessageBox::Icon icon) :
    QMessageBox(parent)
{
    this->setIcon(icon);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowModality(Qt::ApplicationModal);
    this->setStandardButtons(QMessageBox::Ok);

    this->setWindowTitle(title);
    this->setText(content);

    connect(this->timer,
            &QTimer::timeout,
            this,
            [this]()
            {
                if (--remainingSeconds < 0)
                {
                    this->timer->stop();
                    emit this->timedOut();
                    this->close();
                }
            });
}

void NonBlockingMessageBox::showWithTimeout(const int timeoutInSeconds)
{
    this->remainingSeconds = timeoutInSeconds;
    this->timer->start(1000);
    this->show();
}
