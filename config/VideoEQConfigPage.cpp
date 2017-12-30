/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2015 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#include "VideoEQConfigPage.h"
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include "../Slider.h"
#include <QList>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtNetwork>
#include <QNetworkReply>
#include <QDoubleSpinBox>
#include <QSizePolicy>
#include <QMessageBox>
#include <QJsonDocument>
#include <QDebug>


VideoEQConfigPage::VideoEQConfigPage(QWidget *parent) :
    QWidget(parent)
{
    mEngine = SWScale;
    mPresetID = 0;
    QGridLayout *gl = new QGridLayout();
    setLayout(gl);

    int r = 0, c = 0;

//    QLabel *label = new QLabel();
//    label->setText(tr("Engine"));
//    mpEngine = new QComboBox();
//    setEngines(QVector<Engine>(1, SWScale));
//    connect(mpEngine, SIGNAL(currentIndexChanged(int)), SLOT(onEngineChangedByUI()));


//    gl->addWidget(label, r, c,1,1);
//    gl->addWidget(mpEngine, r, c+1,1,3);
//    r++;


    struct {
        QSlider **slider;
        QString text;
        int init;
        QDoubleSpinBox **slidertune;
    } sliders[] = {
        { &mpBSlider, tr("Brightness"),0,&mpBSliderT },
        { &mpCSlider, tr("Constrast"),0, &mpCSliderT},
        { &mpHSlider, tr("Hue"),0, &mpHSliderT },
        { &mpSSlider, tr("Saturation"),0, &mpSSliderT},
        { &mpGSlider, tr("GammaRGB"),0, &mpGSliderT},
        { &mpFSSlider, tr("Filter Sharp"),-100, &mpFSSliderT},
        { 0, QString(),0 }
    };
    for (int i = 0; sliders[i].slider; ++i) {
        QLabel *label = new QLabel(sliders[i].text);
        *sliders[i].slider = new Slider();
        QSlider *slider = *sliders[i].slider;
        slider->setOrientation(Qt::Horizontal);
        slider->setTickInterval(2);
        slider->setRange(-100, 100);
        int sliderinit = (sliders[i].init)?sliders[i].init:0;
        slider->setValue(sliderinit);
        *sliders[i].slidertune  = new QDoubleSpinBox(slider);
        QDoubleSpinBox *sliderTune = *sliders[i].slidertune;
        sliderTune->setDecimals(2);
        sliderTune->setSingleStep(.05);
        if (i==5){
            sliderTune->setRange(0., 2.);
            sliderTune->setValue((sliderinit+100)/100.);
            sliderTune->setToolTip("0:2");
        }else{
            sliderTune->setRange(-1., 1.);
            sliderTune->setValue(sliderinit/100.);
            sliderTune->setToolTip("-1:1");
        }
        gl->addWidget(label, r, c);
        gl->addWidget(slider, r, c+1,1,2);
        gl->addWidget(sliderTune, r, c+3,Qt::AlignHCenter);
        r++;
    }
    mpGlobal = new QCheckBox(tr("Global"));
    mpGlobal->setEnabled(false);
    mpGlobal->setChecked(false);
    mpResetButton = new QPushButton(tr("Reset"));
    mpResetButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));

    gl->addWidget(mpGlobal, r, c, Qt::AlignCenter);
    gl->addWidget(mpResetButton, r, c+1,1,3);
    r++;

    mpListPreset = new QComboBox();

    setListPresets();
    initPresets();

    connect(mpListPreset, SIGNAL(currentIndexChanged(int)), SLOT(onListPresetChangedByUI()));
    gl->addWidget(mpListPreset, r, c,1,2);

    mpLoadPreset = new QPushButton(tr("Load Preset"));
    gl->addWidget(mpLoadPreset, r, c+2);

    mpSavePreset = new QPushButton(tr("Save Preset"));
    gl->addWidget(mpSavePreset, r, c+3);
    r++;
    mpResetLocalCongig = new QPushButton(tr("Reset Local .config"));
    gl->addWidget(mpResetLocalCongig, r, c,1,2);

    mpSaveLocalCongig = new QPushButton(tr("Save Local .config"));
    gl->addWidget(mpSaveLocalCongig, r, c+2,1,2);

    //gl->setColumnMinimumWidth(1,300);


    connect(mpBSlider, SIGNAL(valueChanged(int)), SIGNAL(brightnessChanged(int)));
    connect(mpCSlider, SIGNAL(valueChanged(int)), SIGNAL(contrastChanged(int)));
    connect(mpHSlider, SIGNAL(valueChanged(int)), SIGNAL(hueChanegd(int)));
    connect(mpSSlider, SIGNAL(valueChanged(int)), SIGNAL(saturationChanged(int)));
    connect(mpGSlider, SIGNAL(valueChanged(int)), SIGNAL(gammaRGBChanged(int)));
    connect(mpFSSlider, SIGNAL(valueChanged(int)), SIGNAL(filterSharpChanged(int)));

    connect(mpBSliderT, SIGNAL(valueChanged(double)), SLOT(brightnessTChanged(double)));
    connect(mpCSliderT, SIGNAL(valueChanged(double)), SLOT(contrastTChanged(double)));
    connect(mpHSliderT, SIGNAL(valueChanged(double)), SLOT(hueTChanegd(double)));
    connect(mpSSliderT, SIGNAL(valueChanged(double)), SLOT(saturationTChanged(double)));
    connect(mpGSliderT, SIGNAL(valueChanged(double)), SLOT(gammaRGBTChanged(double)));
    connect(mpFSSliderT, SIGNAL(valueChanged(double)), SLOT(filterSharpTChanged(double)));

    connect(mpGlobal, SIGNAL(toggled(bool)), SLOT(onGlobalSet(bool)));
    connect(mpResetButton, SIGNAL(clicked()), SLOT(onReset()));
    connect(mpSavePreset, SIGNAL(clicked()), SLOT(onSavePreset()));
    connect(mpLoadPreset, SIGNAL(clicked()), SLOT(onLoadPreset()));

    connect(mpResetLocalCongig, SIGNAL(clicked()), SLOT(onResetByZerro()));
    connect(mpSaveLocalCongig, SIGNAL(clicked()), SLOT(onSaveLocalCongig()));
}

void VideoEQConfigPage::onGlobalSet(bool g)
{
    Q_UNUSED(g);
}

void VideoEQConfigPage::setListPresets()
{
    const QList <QString> PresetName =  QList <QString>() << "Day" << "Nigth" << "User 1" << "User 2";
    ColorSpacePreset ctmp;
    mpListPreset->clear();
    for (int i=0;i<PresetName.size();i++) {
         mpListPreset->addItem(QString(" Preset [%1]").arg(PresetName.at(i)));
         ctmp.name=PresetName.at(i);
         mPreset.append(ctmp);
    }
}

void VideoEQConfigPage::initPresets()
{
    for (int i=0;i<mPreset.size();i++){
        fillPreset(i);
    }
}

void VideoEQConfigPage::loadPresets()
{
    for (int i=0;i<mPreset.size();i++){
        //fillPreset(i);
    }
}

void VideoEQConfigPage::savePresets()
{
    for (int i=0;i<mPreset.size();i++){
        //fillPreset(i);
    }
}

void VideoEQConfigPage::fillPreset(int id)
{
    mPreset[id].brightness=brightness_p();
    mPreset[id].contrast=contrast_p();
    mPreset[id].hue=hue_p();
    mPreset[id].saturation=saturation_p();
    mPreset[id].gammaRGB=gammaRGB_p();
    mPreset[id].filterSharp=filterSharp_p();
}

void VideoEQConfigPage::fillCurrentPreset()
{
    fillPreset(mPresetID);
}

void VideoEQConfigPage::readCurrentPreset()
{
    //qDebug("VideoEQConfigPage::readCurrentPreset brightness()  %f, ID: %d",mPreset[mPresetID].brightness, mPresetID);
    brightness(mPreset.at(mPresetID).brightness);
    contrast(mPreset.at(mPresetID).contrast);
    hue(mPreset.at(mPresetID).hue);
    saturation(mPreset.at(mPresetID).saturation);
    gammaRGB(mPreset.at(mPresetID).gammaRGB);
    filterSharp(mPreset.at(mPresetID).filterSharp);

    brightness();
    contrast();
    hue();
    saturation();
    gammaRGB();
    filterSharp();
}

void VideoEQConfigPage::setListPreset(int id)
{
    if (mpListPreset->count()) {
        mpListPreset->setCurrentIndex(id);
    }
    emit listPresetChanged();
}

void VideoEQConfigPage::setEngines(const QVector<Engine> &engines)
{
    mpEngine->clear();
    QVector<Engine> es(engines);
    qSort(es);
    mEngines = es;
    foreach (Engine e, es) {
        if (e == SWScale) {
            mpEngine->addItem(QString::fromLatin1("libswscale"));
        } else if (e == GLSL) {
            mpEngine->addItem(QString::fromLatin1("GLSL"));
        } else if (e == XV) {
            mpEngine->addItem(QString::fromLatin1("XV"));
        }
    }
}

void VideoEQConfigPage::setEngine(Engine engine)
{
    if (engine == mEngine)
        return;
    mEngine = engine;
    if (!mEngines.isEmpty()) {
        mpEngine->setCurrentIndex(mEngines.indexOf(engine));
    }
    emit engineChanged();
}

VideoEQConfigPage::Engine VideoEQConfigPage::engine() const
{
    return mEngine;
}

qreal VideoEQConfigPage::brightness() const
{
    //return (qreal)mpBSlider->value()/100.0;
    return brightness_p();
}

qreal VideoEQConfigPage::brightness_p() const
{
    qreal d=((qreal)mpBSlider->value()/100.0);
    //d+=mRemotePreset.brightness;
    mpBSliderT->setValue(d);
    return d;
}

void VideoEQConfigPage::brightness(qreal val)
{
    mpBSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::contrast() const
{
    //return (qreal)mpCSlider->value()/100.0;
    return contrast_p();
}

qreal VideoEQConfigPage::contrast_p() const
{
    qreal d=((qreal)mpCSlider->value()/100.0);
    mpCSliderT->setValue(d);
    return d;
}

void VideoEQConfigPage::contrast(qreal val) const
{
    mpCSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::hue() const
{
   //return (qreal)mpHSlider->value()/100.0;
   return hue_p();
}

qreal VideoEQConfigPage::hue_p() const
{
    qreal d=((qreal)mpHSlider->value()/100.0);
    mpHSliderT->setValue(d);
    return d;
}

void VideoEQConfigPage::hue(qreal val) const
{
    mpHSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::saturation() const
{
    //return (qreal)mpSSlider->value()/100.0;
    return saturation_p();
}

qreal VideoEQConfigPage::saturation_p() const
{
    qreal d = ((qreal)mpSSlider->value()/100.0);
    mpSSliderT->setValue(d);
    return d;
}

void VideoEQConfigPage::saturation(qreal val) const
{
    mpSSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::gammaRGB() const
{
    return gammaRGB_p();
}

qreal VideoEQConfigPage::gammaRGB_p() const
{
    qreal d=((qreal)mpGSlider->value()/100.0);
    mpGSliderT->setValue(d);
    return d;
}

void VideoEQConfigPage::gammaRGB(qreal val) const
{
    mpGSlider->setValue(int(val*100));
}

qreal VideoEQConfigPage::filterSharp() const
{
    //qDebug("VideoEQConfigPage::filterSharp return bar:%f bar+remote:%f,remote: %f",filterSharp_p(),filterSharp_p()+mRemotePreset.filterSharp,mRemotePreset.filterSharp);
    return filterSharp_p();
}

qreal VideoEQConfigPage::filterSharp_p() const
{
    qreal d = ((qreal)mpFSSlider->value()/100.0);
    mpFSSliderT->setValue(d+1.0);
    return d;
}

void VideoEQConfigPage::filterSharp(qreal val) const
{
    mpFSSlider->setValue(int(val*100));
}

void VideoEQConfigPage::onReset()
{
    if (!resetByRemotePreset()) onResetLocalCongig();
}

void VideoEQConfigPage::onResetByZerro()
{
    mpBSlider->setValue(0);
    mpCSlider->setValue(0);
    mpHSlider->setValue(0);
    mpSSlider->setValue(0);
    mpGSlider->setValue(0);
    mpFSSlider->setValue(-100);
}


void VideoEQConfigPage::onLoadPreset()
{
  qDebug("VideoEQConfigPage::LoadPreset");
  readCurrentPreset();
  saveLocalPresets();
}

void VideoEQConfigPage::onSavePreset()
{
  qDebug("VideoEQConfigPage::SavePreset");
  fillCurrentPreset();
  saveLocalPresets();
}

void VideoEQConfigPage::setSaveFile(const QString &file){
     mFile = file;
}

QString VideoEQConfigPage::saveFile() const
{
    return mFile;
}

void VideoEQConfigPage::loadLocalPresets()
{
    //qDebug("************ Load config presets %s************", qPrintable(mFile));
    QSettings settings(mFile, QSettings::IniFormat);
    for (int i=0;i<mPreset.size();i++){
        settings.beginGroup(QString("preset_%1").arg(i));
        mPreset[i].name=settings.value("name", mPreset[i].name).toString();
        mPreset[i].brightness=settings.value("brightness", mPreset[i].brightness).toReal();
        mPreset[i].contrast=settings.value("contrast", mPreset[i].contrast).toReal();
        mPreset[i].hue=settings.value("hue",  mPreset[i].hue).toReal();
        mPreset[i].saturation=settings.value("saturation", mPreset[i].saturation).toReal();
        mPreset[i].gammaRGB=settings.value("gammaRGB", mPreset[i].gammaRGB).toReal();
        mPreset[i].filterSharp=settings.value("filterSharp", mPreset[i].filterSharp).toReal()-1.;
        settings.endGroup();
    }
    settings.beginGroup("preset");
    mPresetID=settings.value("last",mPresetID).toInt();
    settings.endGroup();
    readCurrentPreset();
    setListPreset(mPresetID);
}

void VideoEQConfigPage::saveLocalPresets()
{
    //qDebug("************ Save config presets %s************", qPrintable(mFile));
    QSettings settings(mFile, QSettings::IniFormat);
    for (int i=0;i<mPreset.size();i++){
        settings.beginGroup(QString("preset_%1").arg(i));
        settings.setValue("name", mPreset.at(i).name);
        settings.setValue("brightness", mPreset.at(i).brightness);
        settings.setValue("contrast", mPreset.at(i).contrast);
        settings.setValue("hue", mPreset.at(i).hue);
        settings.setValue("saturation", mPreset.at(i).saturation);
        settings.setValue("gammaRGB", mPreset.at(i).gammaRGB);
        settings.setValue("filterSharp", mPreset.at(i).filterSharp+qreal(1.));
        settings.endGroup();
    }
    settings.beginGroup("preset");
    settings.setValue("last", mPresetID);
    settings.endGroup();
}

QList <VideoEQConfigPage::ColorSpacePreset> VideoEQConfigPage::presetItems() const
{
    return mPreset;
}

void VideoEQConfigPage::onListPresetChangedByUI()
{
 //   if (mpListPreset->currentIndex() >= mpListPreset.count() || mpListPreset->currentIndex() < 0)
 //       return;
    mPresetID = mpListPreset->currentIndex();
    //qDebug("VideoEQConfigPage::onListPresetChangedByUI : %d",mPresetID);
    emit listPresetChanged();
}

void VideoEQConfigPage::setRemoteUrlPresset(const QString &file){
    if (!file.isEmpty()){
     mURL=file;
    }
}

void VideoEQConfigPage::parseJsonPressetData (QString &strReply){
    if ( strReply.isEmpty()) return;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    if (!jsonResponse.isEmpty()){
        QJsonObject jsonObject = jsonResponse.object();
        if (!jsonObject.isEmpty()){
            if (jsonObject.value("success").toInt()==1){
                mRemotePreset.brightness=jsonObject.value("brightness").toDouble();
                mRemotePreset.contrast=jsonObject.value("contrast").toDouble();
                mRemotePreset.hue=jsonObject.value("hue").toDouble();
                mRemotePreset.saturation=jsonObject.value("saturation").toDouble();
                mRemotePreset.gammaRGB=jsonObject.value("gammaRGB").toDouble();
                mRemotePreset.filterSharp=jsonObject.value("filterSharp").toDouble();
                mRemotePreset.loaded=true;
                qDebug(" *********  mLoadPreset json loaded");
                qDebug("b=%f,c=%f,h=%f,s=%f,g=%f,s=%f",mRemotePreset.brightness,mRemotePreset.contrast,mRemotePreset.hue,mRemotePreset.saturation,mRemotePreset.gammaRGB,mRemotePreset.filterSharp);
                presetUrl=mURL;
                resetByRemotePreset();
                emit engineChanged();
            }
        }
    }
}

bool VideoEQConfigPage::resetByRemotePreset(){

    if (mRemotePreset.loaded){
        mpBSliderT->setValue(mRemotePreset.brightness);
        mpCSliderT->setValue(mRemotePreset.contrast);
        mpHSliderT->setValue(mRemotePreset.hue);
        mpSSliderT->setValue(mRemotePreset.saturation);
        mpGSliderT->setValue(mRemotePreset.gammaRGB);
        mpFSSliderT->setValue(mRemotePreset.filterSharp);
        return true;
    }
    return false;
}


void VideoEQConfigPage::getLocalPressets (){
    if ( mURL.isEmpty()) return;
    QFile f(mURL);
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Can not load local preset %s: %s", f.fileName().toUtf8().constData(), f.errorString().toUtf8().constData());
        return;
    }
    QString strReply = (QString)f.readAll();
    f.close();
    parseJsonPressetData(strReply);
}

void VideoEQConfigPage::getRemotePressets (){
    qDebug("VideoEQConfigPage::getRemotePressets before: mURL %s",qPrintable(mURL));
    if (mURL.compare(presetUrl)==0 || mURL.isEmpty()) return; //check if pressets was loaded for this url
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(onPresetRequestFinished(QNetworkReply*)));
    QUrl url(mURL);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    if (reply->error() != QNetworkReply::NoError) {
        qDebug("Error  getRemotePressets : %s, error %d",qPrintable(mURL),(int)reply->error());
    }
    //qDebug("VideoEQConfigPage::getRemotePressets after: mURL %s",qPrintable(mURL));
}

void VideoEQConfigPage::onPresetRequestFinished(QNetworkReply* reply){
    if(reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        parseJsonPressetData(strReply);
    } else {
        qDebug("VideoEQConfigPage.cpp: ERROR, read fromJson VideoEQConfigPage::onPresetRequestFinished");
    }
    delete reply;
    qDebug("VideoEQConfigPage::onPresetRequestFinished");
}

void VideoEQConfigPage::onEngineChangedByUI()
{
    if (mpEngine->currentIndex() >= mEngines.size() || mpEngine->currentIndex() < 0)
        return;
    mEngine = mEngines.at(mpEngine->currentIndex());
    mpGSlider->setEnabled(mEngine==GLSL);
    mpFSSlider->setEnabled(mEngine==GLSL);
    emit engineChanged();
}



void VideoEQConfigPage::brightnessTChanged(double d){
    mpBSlider->setValue(d*100);
}

void VideoEQConfigPage::contrastTChanged(double d){
    mpCSlider->setValue(d*100);

}

void VideoEQConfigPage::hueTChanegd(double d){
    mpHSlider->setValue(d*100);

}

void VideoEQConfigPage::saturationTChanged(double d){
    mpSSlider->setValue(d*100);

}

void VideoEQConfigPage::gammaRGBTChanged(double d){
    mpGSlider->setValue(d*100);

}
void VideoEQConfigPage::filterSharpTChanged(double d){
    mpFSSlider->setValue((d*100)-100);
}

void VideoEQConfigPage::onResetLocalCongig(){
    if ( mURL.isEmpty()) return;
    QFile f(mURL);
    if (!f.exists()) {
        QString message=tr("Can't find local .config into movie directory.\n%1").arg(f.fileName().toUtf8().constData());
        qWarning()<<message;
        QMessageBox::warning(NULL, tr("Warning"), message);
        return;
    }else if (!f.open(QIODevice::ReadWrite)) {
        QString message=tr("Can't open writable local .config from movie directory.\n%1\n%2").arg(f.fileName().toUtf8().constData()).arg(f.errorString().toUtf8().constData());
        qWarning()<<message;
        QMessageBox::warning(NULL, tr("Warning"), message);
        f.close();
        return;
    }else if (!f.remove()){
        QString message=tr("Can't remove local .config from movie directory.\n%1\n%2").arg(f.fileName().toUtf8().constData()).arg(f.errorString().toUtf8().constData());
        qWarning()<<message;
        QMessageBox::warning(NULL, tr("Warning!"), message);
        f.close();
        return;
    }
    mRemotePreset=ColorSpacePreset(); // initialize mRemotePreset values by default //TODO CheckIT
    reReadColorsCongig();
    QString message=tr("Local .config was cleared.\n").append(f.fileName().toUtf8().constData());
    qWarning()<<message;
    QMessageBox::information(NULL, tr("Information!"), message);

}

void VideoEQConfigPage::onSaveLocalCongig(){
    if ( mURL.isEmpty()) return;
    QFile f(mURL);
    if (!f.open(QIODevice::WriteOnly)) {
        f.close();
        QString message=tr("Can't save local .config into movie directory.\n%1\n%2").arg(f.fileName().toUtf8().constData()).arg(f.errorString().toUtf8().constData());
        qWarning()<<message;
        QMessageBox::warning(NULL, tr("Warning!"), message);
        return;
    }
    QJsonObject jsonObject;
    jsonObject["brightness"]  = brightness_p();
    jsonObject["contrast"]    =  contrast_p();
    jsonObject["hue"]         =  hue_p();
    jsonObject["saturation"]  =  saturation_p();
    jsonObject["gammaRGB"]    =  gammaRGB_p();
    jsonObject["filterSharp"] =  filterSharp_p()+1.0;
    jsonObject["success"]     =  1.;
    QJsonDocument *jsonDoc = new QJsonDocument(jsonObject);
    f.write(jsonDoc->toJson(QJsonDocument::Compact));
    f.close();
    QString message=tr("Local .config was saved into movie directory.\n").append(f.fileName().toUtf8().constData());
    QMessageBox::information(NULL, tr("Information!"), message);
}

void VideoEQConfigPage::reReadColorsCongig(){
    emit brightnessChanged(0);
    emit contrastChanged(0);
    emit hueChanegd(0);
    emit saturationChanged(0);
    emit gammaRGBChanged(0);
    emit filterSharpChanged(0);
}
