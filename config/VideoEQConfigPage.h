/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2014 Wang Bin <wbsecg1@gmail.com>

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

#ifndef VIDEOEQCONFIGPAGE_H
#define VIDEOEQCONFIGPAGE_H

#include <QWidget>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QPushButton;
class QSlider;
class QDoubleSpinBox;
QT_END_NAMESPACE
class VideoEQConfigPage : public QWidget
{
    Q_OBJECT
public:
    enum Engine {
        SWScale,
        GLSL,
        XV,
    };
    struct ColorSpacePreset {
        QString name;
        qreal brightness;
        qreal contrast;
        qreal hue;
        qreal saturation;
        qreal gammaRGB;
        qreal filterSharp;
        bool loaded;
        ColorSpacePreset() : name(""),brightness(0.0),contrast(0.0),hue(0.0),saturation(0.0),gammaRGB(0.0),filterSharp(0.0),loaded(false) {}
    };
    explicit VideoEQConfigPage(QWidget *parent = 0);

    void setEngines(const QVector<Engine>& engines);
    void setListPreset(int id);
    void setListPresets();
    void fillCurrentPreset();
    void readCurrentPreset();
    void fillPreset(int id);
    void initPresets();
    void loadPresets();
    void savePresets();
    void setSaveFile(const QString &file);
    QList<ColorSpacePreset> presetItems() const;
    QString saveFile() const;
    void loadLocalPresets();
    void saveLocalPresets();
    void setRemoteUrlPresset(const QString &file);
    void getRemotePressets ();
    void getLocalPressets ();

    void setEngine(Engine engine);

    Engine engine() const;

    qreal brightness() const;
    qreal contrast() const;
    qreal hue() const;
    qreal saturation() const;
    qreal gammaRGB() const;
    qreal filterSharp() const;
    void brightness(qreal val);
    void contrast(qreal val) const;
    void hue(qreal val) const;
    void saturation(qreal val) const;
    void gammaRGB(qreal val) const;
    void filterSharp(qreal val) const;



    void setXunoVersion(const QString &value);

signals:
    void engineChanged();
    void brightnessChanged(int);
    void contrastChanged(int);
    void hueChanegd(int);
    void saturationChanged(int);
    void listPresetChanged();
    void gammaRGBChanged(int);
    void filterSharpChanged(int);

private slots:
    void onGlobalSet(bool);
    void onReset();
    void onResetByZerro();
    void onEngineChangedByUI();
    void onLoadPreset();
    void onSavePreset();
    void onListPresetChangedByUI();
    void onPresetRequestFinished(QNetworkReply* reply);
    void onResetLocalCongig();
    void onSaveLocalCongig();

    void brightnessTChanged(double);
    void contrastTChanged(double);
    void hueTChanegd(double);
    void saturationTChanged(double);
    void gammaRGBTChanged(double);
    void filterSharpTChanged(double);
    bool resetByRemotePreset();


private:
    qreal brightness_p() const;
    qreal contrast_p() const;
    qreal hue_p() const;
    qreal saturation_p() const;
    qreal gammaRGB_p() const;
    qreal filterSharp_p() const;
    QCheckBox *mpGlobal;
    QComboBox *mpEngine, *mpListPreset;
    QSlider *mpBSlider, *mpCSlider, *mpSSlider, *mpGSlider, *mpFSSlider,*mpHSlider;
    QDoubleSpinBox *mpBSliderT, *mpCSliderT, *mpSSliderT, *mpGSliderT, *mpFSSliderT,*mpHSliderT;
    QPushButton *mpResetButton, *mpSavePreset, *mpLoadPreset, *mpSaveLocalCongig, *mpResetLocalCongig ;
    Engine mEngine;
    QVector<Engine> mEngines;
    int mPresetID;
    QList <ColorSpacePreset> mPreset;
    ColorSpacePreset mRemotePreset;
    QString mFile,mURL,presetUrl;
    void parseJsonPressetData(QString &strReply);
    void reReadColorsCongig();
    QString XunoVersion;
};
#endif // VIDEOEQCONFIGPAGE_H
