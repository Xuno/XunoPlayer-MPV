#include <QApplication>
#include "XunoPlayerMPV.h"
#include "DarkStyle.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Qt sets the locale in the QApplication constructor, but libmpv requires
    // the LC_NUMERIC category to be set to "C", so change it back.

    // style our application with custom dark style
    a.setStyle(new DarkStyle);

    setlocale(LC_NUMERIC, "C");
    XunoPlayerMpv w;
    w.show();

    return a.exec();
}
