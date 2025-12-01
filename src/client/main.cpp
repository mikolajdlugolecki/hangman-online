#include "Client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Client window;
    window.show();
    return app.exec();
}
