#include <QApplication>
#include "XunoPlayerMPV.h"
#include "DarkStyle.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Qt sets the locale in the QApplication constructor, but libmpv requires
    // the LC_NUMERIC category to be set to "C", so change it back.


    QCoreApplication::setApplicationName("XunoPlayer-MPV");
    QCoreApplication::setOrganizationName("Aaex Corp. www.xuno.com.");
    QCoreApplication::setApplicationVersion(VERGIT);
    QCommandLineParser parser;

    parser.setApplicationDescription("XunoPlayer-MPV. Aaex Corp. www.xuno.com.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url", "The URL to open.");
    parser.process(a);

    // style our application with custom dark style
    a.setStyle(new DarkStyle);

    setlocale(LC_NUMERIC, "C");
    XunoPlayerMpv w;
    w.show();

    if (!parser.positionalArguments().isEmpty()) {
        QList<QUrl> urls;
        foreach (const QString &a, parser.positionalArguments())
            urls.append(QUrl::fromUserInput(a, QDir::currentPath(), QUrl::DefaultResolution));
        w.OpenAndPlay(urls.at(0));
    }

    return a.exec();
}
