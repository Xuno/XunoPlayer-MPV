#include "configwebmemu.h"
#include "common/Config.h"


ConfigWebMemu::ConfigWebMemu(QWidget *parent) :
    QMenu(parent)
{
    initMenuItems();
    init();
}

ConfigWebMemu::~ConfigWebMemu()
{
    if (mpXunoBrowser) {
        delete mpXunoBrowser;
        mpXunoBrowser = 0;
    }
}

void ConfigWebMemu::openurl(QAction * a){
    int i=a->data().toInt();
    qDebug() << i << menuitemList.at(i)->name << menuitemList.at(i)->url;
    if (!QUrl(menuitemList.at(i)->url).host().isEmpty())
        onXunoBrowser(menuitemList.at(i)->url);
}

void ConfigWebMemu::initMenuItems(){

    QMap<QString,QVariant> links = QMap<QString,QVariant>(Config::instance().WebLinks());
    QStringList names = links.keys();
    QList<QVariant> urls = links.values();

    menuitemList.clear();
    //For Fixed list items
    //menuitemList.append(new MenuItems({"Xuno","http://www.xuno.com/playlist_8bit.php",":/www.xuno.net.ico"}));
    //menuitemList.append(new MenuItems({"Google","https://www.google.com",":/www.google.com.ico"}));

    if (names.size()){
        for (int i = 0; i < names.size(); ++i){
            QString iconlink="";
            QString urli=urls[i].toString();
            if (QUrl(urli).host().startsWith("www.xuno.com")){
                iconlink= ":/www.xuno.net.ico";
            }else if (QUrl(urli).host().startsWith("www.google.com")){
                iconlink= ":/www.google.com.ico";
            }
            menuitemList.append(new MenuItems({names[i],urli ,iconlink}));
        }
    }else{
        menuitemList.append(new MenuItems({tr("Empty list, please define it in setup"),"" ,""}));
    }
    XUNOserverUrl="http://www.xuno.com";
    XUNOpresetUrl=XUNOserverUrl+"/getpreset.php?";
}

void ConfigWebMemu::init(){
    clear();
    for (int i=0;i<menuitemList.size();i++){
        QString iconurl=menuitemList.at(i)->iconurl;
        if (iconurl.isEmpty()){
         QUrl iurl= QUrl(QString(menuitemList.at(i)->url).append("/favicon.ico"));
        }
        QIcon* ic = new QIcon(iconurl);
        QAction* qa = addAction(*ic,menuitemList.at(i)->name);
        qa->setData(i);
    }
    connect(this, SIGNAL(triggered(QAction *)), this, SLOT(openurl(QAction *)));
}


void ConfigWebMemu::onXunoBrowser(QString url){

    bool emp=url.isEmpty();

    if (!mpXunoBrowser){
        mpXunoBrowser = new XunoBrowser(Q_NULLPTR,XunoVersion);
        mpXunoBrowser->setXUNOContentUrl(QString(XUNOserverUrl).append("/content/"));
        connect(mpXunoBrowser, SIGNAL(clicked()), SLOT(onClickXunoBrowser()));
    }
    if (mpXunoBrowser->isHidden()) mpXunoBrowser->show();

    if (emp) {
      mpXunoBrowser->setUrl(QUrl(QString(XUNOserverUrl).append("/playlist_12bit.php")));
    }else{
      mpXunoBrowser->setUrl(url);
    }
}

void ConfigWebMemu::onClickXunoBrowser(){
    QUrl url=mpXunoBrowser->getClikedUrl();
    emit onPlayXunoBrowser(url);
}

void ConfigWebMemu::setXunoVersion(const QString &value)
{
    XunoVersion = value;
}


void ConfigWebMemu::onChanged(){
    initMenuItems();
    init();
}

