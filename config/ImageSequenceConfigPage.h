#ifndef IMAGESEQUENCECONFIGPAGE_H
#define IMAGESEQUENCECONFIGPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QTime>
#include <QFileInfo>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCore>
#include <QRegularExpression>
#include <QDialogButtonBox>
#include <QCheckBox>


class QPushButton;
class QSpinBox;

class ImageSequenceConfigPage : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool EnableFrameExtractor READ getEnableFrameExtractor WRITE setEnableFrameExtractor)

public:
    explicit ImageSequenceConfigPage(QWidget *parent = 0);
    void openFiles();
    void setMovieDuration(qint64 d);
    void setStartFrame(quint32 n);
    void setEndFrame(quint32 n);
    void setImageSequenceFileName(QString fname);
    void setRepeatLoop(bool loop);
    void setEnableFrameExtractor(bool s);
    bool getEnableFrameExtractor() const;


signals:
    void play(QString);
    void stop();
    void repeatAChanged(QTime);
    void repeatBChanged(QTime);
    void toggleRepeat(bool);
    void customfpsChanged(double);
    void toogledFrameExtractor(bool state);
    void setPlayerScale(double);
    void RepeatLoopChanged(int i);


public slots:
    void onSelectImgages();
    void setFPS(double n);
    void setTotalFrames(int n);

private slots:
    void on_checkBoxExtractor_toggled(bool state);
    void on_checkLoop_toggled(bool state);

    void on_InputStartFrame_valueChanged(int arg1);
    void on_InputEndFrame_valueChanged(int arg1);
    void on_InputTotalFrame_valueChanged(int arg1);
    void on_cbDecodeGeometryFromFileName_toggled(bool checked);
    void on_InputPath_textChanged(const QString &text);
    void playImgages();

private:
    QSpinBox *mpTotalFramesBox;
    QDoubleSpinBox *mpFpsBox;
    QPushButton *mpSelectImgButton, *mpPlayImgButton;
    quint64 startPos,stopPos;
    double fps;
    quint32 startFrame, frames, totalAllInputFrames;
    QFileInfo fileinfo;
    QPushButton *buttonPlay;
    QLineEdit *InputPath;
    QSpinBox *InputStartFrame, *InputEndFrame, *InputTotalFrame;
    QLabel *InputAllTotalFrame;
    QCheckBox *checkLoop;
    QComboBox *cbColorTypeInput;
    QPushButton *checkBoxExtractor;
    QDoubleSpinBox *InputScale;
    QCheckBox *cbDecodeGeometryFromFileName;
    QSpinBox *inputImageW;
    QSpinBox *inputImageH;


    void calculatePos();
    void analyzeFilename();
    int getDigetsFilename();
    bool playing_start;

    const QString dataImageSeparator="_";

    const QStringList ImageTypes = QStringList()
    <<  "420"
    <<  "422"
    <<  "RGB"
    <<  "RGBA"
    <<  "bmp"
    <<  "cr2"
    <<  "dng"
    <<  "dpx"
    <<  "exr"
    <<  "jp2"
    <<  "png"
    <<  "jpg"
    <<  "tga"
    <<  "tif"
    <<  "tiff";

    int getDigetsFilename(QString filename);
    int getNumberFilename(QString filename);
    int getTotalNumberFilename(QString filename);
    QString getSequenceFilename(QString filename);
    void updateInputTotalFrameValue();
    void updateInputEndFrameValue();
    void getGeometryFromFilename(QString filename, QSize &size, int &depth);

};

#endif // IMAGESEQUENCECONFIGPAGE_H
