#include <QApplication>

#include "railfie.h"

int main(int argc, char *argv[])
{
    QApplication app{argc, argv};

    Railfie railfie;
    railfie.show();

    return app.exec();
}
