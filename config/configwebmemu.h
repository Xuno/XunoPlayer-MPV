#ifndef CONFIGWEBMEMU_H
#define CONFIGWEBMEMU_H

#include <QWidget>
#include <QMenu>
#include <QtDebug>
#include "XunoBrowser.h"

class ConfigWebMemu : public QMenu
{
    Q_OBJECT

public:
    explicit ConfigWebMemu(QWidget *parent = nullptr);
    ~ConfigWebMemu();

    void setXunoVersion(const QString &value);

signals:
    void onPlayXunoBrowser(QUrl url);

public slots:
 void onChanged();

private slots:
 void openurl(QAction *a);
 void onClickXunoBrowser();

private:
 XunoBrowser* mpXunoBrowser = nullptr;
 QString XUNOserverUrl, XUNOpresetUrl, XunoVersion;

 struct MenuItems {
     QString name;
     QString url;
     QString iconurl;
 };
 QList <MenuItems *> menuitemList;

 void init();
 void initMenuItems();
 void onXunoBrowser(QString url);
};


#endif // CONFIGWEBMEMU_H
