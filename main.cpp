#include "Application/startscreen.h"
#include "libs/Utilities/global.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    if (qgetenv("QT_FONT_DPI").isEmpty()) {
        qputenv("QT_FONT_DPI", "84");
    };
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/resources/Icon256x256.png"));
    Global();
    StartScreen w;
    w.show();
    return a.exec();
}
