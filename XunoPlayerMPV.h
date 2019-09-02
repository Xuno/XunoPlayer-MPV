#ifndef XunoPlayerMpv_H
#define XunoPlayerMpv_H

#include <QWidget>
#include <QDebug>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QTimeEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QTimer>
#include <QComboBox>
#include <QToolTip>

#ifndef unix
#include <QtWinExtras/QWinThumbnailToolBar>
#include <QtWinExtras/QWinThumbnailToolButton>
#include <QtWinExtras/QWinJumpList>
#include <QtWinExtras/QWinJumpListCategory>
#endif

#include "common/ScreenSaver.h"
#include "StatisticsView.h"



#include "Slider.h"
#include "playlist/PlayList.h"
#include "ClickableMenu.h"

#include "config/ConfigDialog.h"
#include "config/configwebmemu.h"
#include "config/VideoEQConfigPage.h"
#include "config/ImageSequenceConfigPage.h"

#include "EventFilter.h"
#include "common/Config.h"
#include "version.h"

class MpvWidget;
class QSlider;
class QPushButton;

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QAbstractButton)
QT_FORWARD_DECLARE_CLASS(QWinTaskbarButton)
QT_FORWARD_DECLARE_CLASS(QWinTaskbarProgress)
QT_FORWARD_DECLARE_CLASS(QWinThumbnailToolBar)
QT_FORWARD_DECLARE_CLASS(QWinThumbnailToolButton)

class XunoPlayerMpv : public QWidget
{
    Q_OBJECT
public:
    explicit XunoPlayerMpv(QWidget *parent = Q_NULLPTR);
    ~XunoPlayerMpv() override;
    void OpenAndPlay(const QUrl &url);
    QString getXunoVersion(int8_t longversion=0) const;

public Q_SLOTS:
    void openMedia();
    void seek();
    void seek(int pos);
    void pauseResume();

    void openFile();


signals:
    void ready();

private Q_SLOTS:
    void setSliderRange(int duration);
    void positionChanged(int pos);
    void on_START_FILE();
    void on_FILE_LOADED();
    void on_PAUSE();
    void on_END_FILE();
    void stopUnload();
    void setVolume();
    void setMute();
    void onClickXunoBrowser(QUrl url);
    void onFullScreen();

    void onTimeSliderLeave();
    void onTimeSliderHover(int pos, int value);
    void handleFullscreenChange();
    void onScaleX1Btn();
    void onScaleX15Btn();
    void onScaleX2Btn();
    void onScaleBtn(qreal _scale);
    void updateThumbnailToolBar();
    void setup();
    void tryHideControlBar();
    void tryShowControlBar();
    void openUrl();
    void initPlayer();
    void processPendingActions();
    void play(QString url);
    void seek_relative(int pos);
    void seekForward();
    void seekBackward();
    void onActionEsc();
    void onVideoEQEngineChanged();
    void onBrightnessChanged(int _brightness);
    void onContrastChanged(int _contrast);
    void onHueChanged(int _hue);
    void onSaturationChanged(int _saturation);
    void onGammaRGBChanged(int _gamma);
    void onFilterSharpChanged(int _sharp);
    bool showInfo(bool hide=false);
    void onImageSequenceConfig();
    void customfpsChanged(double n);
    void showInfoTimer(bool state=true);
    void onRepeatLoopChanged(int i);
    void onSetRepeateMax(int i);
    void onToggleRepeat(bool s);
    void onRepeatAChanged(const QTime &t);
    void onRepeatBChanged(const QTime &t);
private:
    QWidget *m_mpvWidget=Q_NULLPTR;
    MpvWidget *m_mpv=Q_NULLPTR;
    MpvWidget *mpRenderer=Q_NULLPTR;
    QSlider *m_slider=Q_NULLPTR;
    QPushButton *m_openBtn=Q_NULLPTR;
    QPushButton *m_playBtn=Q_NULLPTR;
    QWidget *m_preview=Q_NULLPTR;

private:
    bool mIsReady, mHasPendingPlay;
    bool mControlOn=false,mShifOn=false;
    bool mNaitiveScaleOn=false;
    int mCursorTimer;
    int mShowControl; //0: can hide, 1: show and playing, 2: always show(not playing)
    int mRepeateMax;
    double mCustomFPS=0.;
    double mPlayerScale=1.0;
    QStringList mAudioBackends;
    QVBoxLayout *mpPlayerLayout;

    QWidget *m_mpvLayoutWidget=Q_NULLPTR;
    QWidget *m_ControlLayoutWidget=Q_NULLPTR;
    QWidget *detachedControl=Q_NULLPTR;
    QVBoxLayout *detachedControlLayout=Q_NULLPTR;
    QWidget *mpControl;
    QLabel *mpCurrent, *mpEnd;
    QLabel *mpTitle;
    QLabel *mpSpeed;
    Slider *mpTimeSlider=Q_NULLPTR, *mpVolumeSlider=Q_NULLPTR;
    QToolButton *mpWebBtn, *mpFullScreenBtn, *mpScaleX2Btn, *mpScaleX15Btn,*mpScaleX1Btn;
    QToolButton *mpVolumeBtn;
    QToolButton *mpPlayPauseBtn=Q_NULLPTR;
    QToolButton *mpStopBtn, *mpForwardBtn, *mpBackwardBtn;
    QToolButton *mpOpenBtn;
    QToolButton *mpInfoBtn, *mpMenuBtn, *mpSetupBtn, *mpCaptureBtn;
    QMenu *mpMenu;
    QAction *mpVOAction, *mpARAction; //remove mpVOAction if vo.id() is supported
    QAction *mpRepeatEnableAction;
    QCheckBox *mpRepeatLoop;
    QWidgetAction *mpRepeatAction;
    QSpinBox *mpRepeatBox;
    QTimeEdit *mpRepeatA, *mpRepeatB;
    QAction *mpAudioTrackAction;
    QMenu *mpAudioTrackMenu;
    QMenu *mpChannelMenu;
    QMenu *mpClockMenu = Q_NULLPTR;
    QActionGroup *mpClockMenuAction = Q_NULLPTR;
    QAction *mpChannelAction;
    QList<QAction*> mVOActions;

//    QtAV::AVClock *mpClock;
//    QtAV::AVPlayer *mpPlayer;
//    QtAV::VideoRenderer *mpRenderer;
//    QtAV::LibAVFilterVideo *mpVideoFilter;
//    QtAV::LibAVFilterAudio *mpAudioFilter;
    QString mFile;
    QString mTitle;

    QLabel *mpPreview;

//    DecoderConfigPage *mpDecoderConfigPage;
    VideoEQConfigPage *mpVideoEQ;

    PlayList *mpPlayList, *mpHistory;

    QPointF mGlobalMouse;
    StatisticsView *mpStatisticsView = Q_NULLPTR;

//    OSDFilter *mpOSD;
//    QtAV::SubtitleFilter *mpSubtitle;
//    QtAV::VideoPreviewWidget *m_preview;

    QString XUNOserverUrl;
    QString XUNOpresetUrl;

    ImageSequenceConfigPage *mpImageSequence = Q_NULLPTR;
    ConfigWebMemu *mpWebMenu = Q_NULLPTR;

    //ImgSeqExtractControl *mpImgSeqExtract=0;
    QString aboutXunoQtAV_PlainText();
    QString aboutXunoQtAV_HTML();

    //ShaderFilterXuno *shaderXuno=Q_NULLPTR;
    //SaveGLXuno *mSaveGLXuno=Q_NULLPTR;

    //XunoGLSLFilter *mpGLSLFilter=Q_NULLPTR;

    //QtAV::DynamicShaderObject *m_shader;
    //QtAV::GLSLFilter *m_glsl;


    bool needToUseSuperResolution=false;
    bool needToUseSuperResolutionLastLinearFiltering=true;
    bool needToUseFXAAFiltering=false;

#ifndef unix
    QWinThumbnailToolBar *thumbnailToolBar=Q_NULLPTR;
    QWinThumbnailToolButton *playToolButton=Q_NULLPTR;
#endif

    void setupUi(QWidget *m_mpv_parent, QWidget *_mpv);

    QUrl remove_fistsubdomain(QUrl url);
    bool same_site_domain(const QUrl &url1, const QUrl &url2);

    void workaroundRendererSize();
    void reSizeByMovie();
    QSize movieSize();
    void setPlayerScale(const double scale);
    void createShortcuts();
    void createThumbnailToolBar();
    void createJumpList();
    void showTextOverMovie(QString txt, int duration=2);
    void seekFrameForward();
    void seekFrameBackward();
    void setMovieSpeed(float speed);
    void setMovieColors(qreal contrast=1.0, qreal brightness=0.0, qreal saturation=1.0, qreal gamma=1.0);
    void contrast(int contrast=0);
    void brightness(int brightness=0);
    void saturation(int saturation=0);
    void hue(int hue=0);
    void gamma(int gamma=0);
    void sharpen(qreal sharpen=0.0);
    void loadRemoteUrlPresset(const QString &url);
    void calcToUseSuperResolution();

    int getGamma();
    qreal getSharpen();
    qreal getContrast();
    qreal getBrightness();
    qreal getSaturation();
    bool isFileImgageSequence();
    QTimer *TimerShowinfo=Q_NULLPTR;
    void setRepeat(int repeat);
    void setVideoTimePlay(QTime start, QTime end);
protected:
    virtual void closeEvent(QCloseEvent *e) override;
    virtual void resizeEvent(QResizeEvent *) override;
    virtual void timerEvent(QTimerEvent *) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void keyReleaseEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // XunoPlayerMpv_H
