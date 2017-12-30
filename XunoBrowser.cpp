/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtWidgets>
#include <QMainWindow>
#include <QTextDocument>
#include <QNetworkProxyFactory>
#include "XunoBrowser.h"

//--------- myWebEnginePage  -----------

bool myWebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    qDebug()<<"myWebEnginePage"<<url<<type<<isMainFrame;
    emit onClick(url);
    return true;
}

//------------------------------------------------------


XunoBrowser::XunoBrowser(QWidget *parent) : QDialog(parent)
  , view(0)
  , loading(0)
{
    progress = 0;

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    this->setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    this->setPalette(pal);
    this->setWindowTitle("XunoBrowser");
    this->setWindowFlags(((this->windowFlags() | Qt::CustomizeWindowHint)&  Qt::WindowMaximizeButtonHint & ~Qt::WindowContextHelpButtonHint) );

    loading = new QLabel("\n   Loading....",this);
    loading->setAlignment(Qt::AlignCenter);
    this->resize(QSize(600,40));
    this->show();

    myWebEnginePage *page = new myWebEnginePage();

    connect(page, SIGNAL(onClick(QUrl)), SLOT(linkClicked(QUrl)));

    view = new QWebEngineView(this);
    view->setPage((QWebEnginePage*)page);
    //connect(view->page(), SIGNAL(urlChanged(QUrl)),SLOT(linkClicked(QUrl)));
    //view->page()->setLinkDelegationPolicy(QWebEnginePage::DelegateAllLinks);
    //TODO There is no way to connect a signal to run C++ code when a link is clicked. However, link clicks can be delegated to the Qt application instead of having the HTML handler engine process them by overloading the QWebEnginePage::acceptNavigationRequest() function. This is necessary when an HTML document is used as part of the user interface, and not to display external data, for example, when displaying a list of results. https://wiki.qt.io/Porting_from_QtWebKit_to_QtWebEngine


    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    //connect(view, SIGNAL(linkClicked(QUrl)), SLOT(linkClicked(QUrl)));
    //TODO There is no way to connect a signal .. https://wiki.qt.io/Porting_from_QtWebKit_to_QtWebEngine


}

XunoBrowser::~XunoBrowser() {
    //QWebEngineSettings::clearMemoryCaches();
    delete view;
}

void XunoBrowser::setUrl(const QUrl &url) {
    if (view->url()!=url) view->load(url);
    if (this->isMinimized()) this->showNormal();
    this->setFocus();
}

void XunoBrowser::setXUNOContentUrl(const QString &url) {
    XUNOContentUrl=url;
}


void XunoBrowser::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle((QString("XunoBrowser : %1").arg(view->title())));
    else
        setWindowTitle(QString("XunoBrowser : %1 (%2%)").arg(view->title()).arg(progress));
}

void XunoBrowser::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void XunoBrowser::adjustBrowserSize()
{
    if (view && view->isHidden()){
        view->resize(QSize(1100,870));
        view->show();
        this->setFocus();
        this->adjustSize();
        int screen=QApplication::desktop()->screenNumber(this);
        this->move(QApplication::desktop()->availableGeometry(screen).center() - this->rect().center());
    }
}


void XunoBrowser::finishLoading(bool)
{
    if (loading) {
        delete loading; // delete loading text...
        loading=0;
    }
    progress = 100;
    adjustTitle();
    adjustBrowserSize();
}

void XunoBrowser::hideEvent(QHideEvent *e)
{
    qDebug("XunoBrowser::hideEvent");
    QDialog::hideEvent(e);
}

void XunoBrowser::showEvent(QShowEvent *e)
{
    qDebug("XunoBrowser::showEvent");
    QDialog::showEvent(e);
}


void XunoBrowser::linkClicked(QUrl url){
    qDebug("XunoBrowser::linkClicked %s",qPrintable(url.toString()));
    if (url.toString().startsWith(XUNOContentUrl) && !url.toString().contains("playlist")){
        qDebug("XunoBrowser::linkClicked pass %s",qPrintable(url.toString()));
        clickedUrl=url;
        this->hide();
        emit clicked();
    }else{
        //        clickedUrl.clear();
        //        view->load(url);
    }
}

QUrl XunoBrowser::getClikedUrl(){
    return clickedUrl;
}

void XunoBrowser::resizeEvent(QResizeEvent* e) {
    qDebug("XunoBrowser::resizeEvent");
    if (view && view->isVisible()) {
        view->resize(this->size());
    }
    QDialog::resizeEvent(e);
}



