#include "WebConfigPage.h"
#include <QLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
//#include <QTextEdit>
#include <QListView>
#include <QGroupBox>
#include "common/Config.h"
#include <QDebug>
#include <QMessageBox>
#include <QUrl>


WebConfigPage::WebConfigPage()
{
    setObjectName("web");
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setAlignment(Qt::AlignTop);
    QGroupBox *groupBox = new QGroupBox(tr("Edit list of Web links"));
    QVBoxLayout *gl = new QVBoxLayout();
    //gl->setSizeConstraint(QLayout::SetFixedSize);

    initModelData();

    m_options = new QListView();
    m_options->setModel(model);
    m_options->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_options, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelected(QModelIndex)));

    gl->addWidget(m_options);

    QHBoxLayout *hb1 = new QHBoxLayout();
    QPushButton *bInsert = new QPushButton(tr("&Insert"), this);
    QPushButton *bChange = new QPushButton(tr("&Change"), this);
    QPushButton *bDelete = new QPushButton(tr("&Delete"), this);
    hb1->addWidget(bInsert);
    hb1->addWidget(bChange);
    hb1->addWidget(bDelete);

    QObject::connect(bInsert, SIGNAL(pressed()), this, SLOT(onInsert()));
    QObject::connect(bChange, SIGNAL(pressed()), this, SLOT(onChange()));
    QObject::connect(bDelete, SIGNAL(pressed()), this, SLOT(onDelete()));

    gl->addLayout(hb1);
    int r=0;
    QGridLayout *gbox = new QGridLayout();
    QLabel *lname = new QLabel(tr("&Name:"));
    ename = new QLineEdit();
    lname->setBuddy(ename);
    gbox->addWidget(lname,r,0);
    gbox->addWidget(ename,r,1);
    r++;
    QLabel *lurl = new QLabel(tr("&Url:"));
    eurl = new QLineEdit();
    lurl->setBuddy(eurl);
    gbox->addWidget(lurl,r,0);
    gbox->addWidget(eurl,r,1);
    gl->addLayout(gbox);
    groupBox->setLayout(gl);
    vbox->addWidget(groupBox);
    setLayout(vbox);
    applyToUi();
}

void WebConfigPage::initModelData()
{
    links = QMap<QString,QVariant>(Config::instance().WebLinks());
    QStringList numbers = links.keys();

    if (!model) model = new StringListModel();
    model->setStringList(numbers);
}


QString WebConfigPage::name() const
{
    return "Web";
}


bool WebConfigPage::ValidURL(const QString &url)
{
    QUrl u(url);
    if (!u.isValid()||u.host().isEmpty()){
        QMessageBox::warning(this, tr("Web link"),
                             tr("     Wrong URL is used. \n"
                                "May be need start from 'http://' ?\n"),
                             QMessageBox::Ok);
        return false;
    }
    return true;
}

void WebConfigPage::onInsert()
{
    //qDebug("WebConfigPage onInsert");
    QString name = ename->text();
    QString url = eurl->text();
    if (name.isEmpty()||url.isEmpty()){
        ename->setFocus();
        QMessageBox::warning(this, tr("Insert new Web link"),
                             tr("\n   Both of fields must be filled.   \n"),
                             QMessageBox::Ok);
    }else if(ValidURL(url)) {
        if (model->match(model->index(0),Qt::DisplayRole,name).isEmpty()){
            model->insertRow(model->rowCount(),QModelIndex());
            QModelIndex index = model->index(model->rowCount()-1, 0, QModelIndex());
            if (model->setData(index, name, Qt::EditRole)){
                m_options->setCurrentIndex(index);
                saveLinks(name,url);
                ename->clear();
                eurl->clear();
            }
        }else{
            QMessageBox::warning(this, tr("Insert new Web link"),
                                 tr("\n   This name is used.     \n"),
                                 QMessageBox::Ok);
        }

    }
}

void WebConfigPage::onChange()
{
    //qDebug("WebConfigPage onChange");
    QModelIndex index = m_options->currentIndex();
    QString name = ename->text();
    QString url = eurl->text();
    if (name.isEmpty()||url.isEmpty()){
        QMessageBox::warning(this, tr("Changing Web link"),
                             tr("\nBoth of fields must be filled.\n"),
                             QMessageBox::Ok);
    }else if(ValidURL(url)) {
        QString oldname =index.data(Qt::DisplayRole).toString();
        if (model->setData(index, name, Qt::EditRole)){
            model->sort(0);
            m_options->setCurrentIndex(index);
            //qDebug("WebConfigPage Changed");
            deleteLinks(oldname);
            saveLinks(name,url);
            ename->clear();
            eurl->clear();
        }
    }
}

void WebConfigPage::onDelete()
{
    qDebug("WebConfigPage onDelete");
    QModelIndex index = m_options->currentIndex();
    if (index.isValid()){
        QString name = index.data().toString();
        int ret=QMessageBox::question(this, tr("Deleting Web link"),
                                      tr("Record '%1' will be deleted.\n   Do you want delete it?").arg(name),
                                      QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
        if (ret==QMessageBox::Yes){
            if (links.remove(name)){
                QModelIndex index = m_options->currentIndex();
                qDebug("Index row %d:",index.row());
                if (model->removeRow(index.row(),QModelIndex())){
                    //qDebug("WebConfigPage Deleted");
                    ename->clear();
                    eurl->clear();
                }
            }
        }
    }else{
        QMessageBox::warning(this, tr("Deleting Web link"),
                             tr("Not selected name in list.\nSelect it before continiue."),
                             QMessageBox::Ok);
    }
}

void WebConfigPage::onSelected(QModelIndex index)
{
    QString name = index.data().toString();
    QString url = links.value(name).toString();
    //qDebug() << "WebConfigPage onSelected :" << name << url ;
    ename->setText(name);
    eurl->setText(url);
}

void WebConfigPage::saveLinks(QString name, QString urls)
{
    links.insert(name,urls);
}

void WebConfigPage::deleteLinks(QString name)
{
    links.remove(name);
}

void WebConfigPage::applyFromUi()
{
      Config::instance().setWebLinks(links);
}

void WebConfigPage::applyToUi()
{
      initModelData();
}
