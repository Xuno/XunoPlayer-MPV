#include "XunoPlayerMPV.h"
#include "mpvwidget.h"




XunoPlayerMpv::XunoPlayerMpv(QWidget *parent) :
    QWidget(parent)
  , mIsReady(false)
  , mHasPendingPlay(false)
  , mControlOn(false)
  , mShowControl(2)
  , mRepeateMax(0)
{
    XUNOserverUrl=QString::fromLatin1("http://www.xuno.com/playlist_10bit.php ");
    XUNOpresetUrl=XUNOserverUrl+QString::fromLatin1("/getpreset.php?");

    QString xunoversion;
#ifdef VERSION_STRING
#ifdef VERGIT
    xunoversion=QString("%1-git:%2").arg(VERSION_STRING).arg(VERGIT);
#endif
#endif

//setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
//#if defined(Q_OS_WIN)
//  setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint /*| Qt::FramelessWindowHint*/ );
//#endif
//setAttribute(Qt::WA_TranslucentBackground);

    setWindowTitle(tr("XunoPlayer-MPV ver: %1").arg(xunoversion));

    setWindowIcon(QIcon(QString::fromLatin1(":/XunoPlayer-MPV_128x128.ico")));
    m_mpvWidget = new QWidget(this);
    m_mpvWidget->resize(960, 540);
    m_mpvWidget->setContentsMargins(0,0,0,0);
    this->setContentsMargins(0,0,0,0);

    m_mpv = new MpvWidget(m_mpvWidget);

    //m_mpvLayoutWidget->resize(m_mpvWidget->size());

    mpRenderer = m_mpv;

    if (m_mpv) {
        setupUi(this,m_mpv);
        connect(m_mpv, &MpvWidget::durationChanged,     this, &XunoPlayerMpv::setSliderRange);
        connect(m_mpv, &MpvWidget::positionChanged,     this, &XunoPlayerMpv::positionChanged);
        connect(m_mpv, &MpvWidget::mpv_on_START_FILE,   this, &XunoPlayerMpv::on_START_FILE);
        connect(m_mpv, &MpvWidget::mpv_on_FILE_LOADED,  this, &XunoPlayerMpv::on_FILE_LOADED);
        connect(m_mpv, &MpvWidget::mpv_on_PAUSE,        this, &XunoPlayerMpv::on_PAUSE);
        connect(m_mpv, &MpvWidget::mpv_on_END_FILE,     this, &XunoPlayerMpv::on_END_FILE);

        QRect g1=m_mpvLayoutWidget->geometry();
        if (m_ControlLayoutWidget){
            QRect g2=m_ControlLayoutWidget->geometry();
            m_ControlLayoutWidget->setGeometry(0,g1.height()-g2.height(),g1.width(),g2.height());
        }
    }



    WindowEventFilter *we = new WindowEventFilter(this);
    installEventFilter(we);

    connect(we, SIGNAL(fullscreenChanged()), SLOT(handleFullscreenChange()));

    //setMouseTracking(true); //mouseMoveEvent without press.
    //installEventFilter(m_mpv);

//    m_mpvLayoutWidget->setMouseTracking(true);

//    m_ControlLayoutWidget->setMouseTracking(true);



    connect(this, SIGNAL(ready()), SLOT(processPendingActions()));

 //   createShortcuts();
//    createThumbnailToolBar();
//    createJumpList();

//    EventFilter *ef = new EventFilter();
//    qApp->installEventFilter(ef);

    qApp->installEventFilter(this);

}

bool XunoPlayerMpv::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
  if (event->type() == QEvent::MouseMove)
  {
    QMouseEvent *e = static_cast<QMouseEvent*>(event);
    //qDebug()<<"eventFilter"<<(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));

    unsetCursor();
    //qDebug()<<"XunuMpvPlayer::mouseMoveEvent"<<e->globalPos()<<e->pos();
    QPoint posG=e->globalPos()-mapToGlobal(QPoint(0,0));
    if (posG.y() > (height() - 65)) {
        //qDebug()<<"XunuMpvPlayer::mouseMoveEvent height()-65"<<height()<<mapToGlobal(QPoint(0,0));
        if (mShowControl == 0) {
            mShowControl = 1;
            tryShowControlBar();
        }
    } else {
        //qDebug()<<"XunuMpvPlayer::mouseMoveEvent up"<<posG.y()<<(height() - 65)<<e->globalPos()<<e->pos();
        if (mShowControl == 1) {
            mShowControl = 0;
            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
        }
    }
  }
  return false;
}

XunoPlayerMpv::~XunoPlayerMpv()
{
    if (m_mpv) {
        //delete m_mpv;
        m_mpv->deleteLater();
        m_mpv = Q_NULLPTR;
    }

    if (m_preview) {
        m_preview->close();
        delete m_preview;
    }

    mpHistory->save();
    mpPlayList->save();

    if (mpVolumeSlider && !mpVolumeSlider->parentWidget()) {
        mpVolumeSlider->close();
        delete mpVolumeSlider;
        mpVolumeSlider = 0;
    }

    if (mpStatisticsView) {
        delete mpStatisticsView;
        mpStatisticsView = 0;
    }

}

void XunoPlayerMpv::openMedia()
{
    QString file = QFileDialog::getOpenFileName(0, tr("Open a video"), Config::instance().lastFile());
    if (file.isEmpty())
        return;
    Config::instance().setLastFile(file);
    play(file);
}

void XunoPlayerMpv::openFile()
{
    QString file = QFileDialog::getOpenFileName(0, tr("Open a media file"), Config::instance().lastFile());
    if (file.isEmpty())
        return;
    Config::instance().setLastFile(file);
    play(file);
}

void XunoPlayerMpv::openUrl()
{
    QString url = QInputDialog::getText(0, tr("Open an url"), tr("Url"));
    if (url.isEmpty())
        return;
    play(url);
}

void XunoPlayerMpv::play(QString url){
    qDebug()<<"MainWindow::play"<<url;
    if (m_mpv && !url.isEmpty()) {
        mFile=url;
        loadRemoteUrlPresset(mFile);
        m_mpv->command(QStringList() << "loadfile" << url);

    }
}

void XunoPlayerMpv::seek()
{
    qDebug()<<"MainWindow::seek";
    //if (m_mpv) m_mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void XunoPlayerMpv::seek(int pos)
{
    qDebug()<<"MainWindow::seek"<<pos;
    if (m_mpv) m_mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void XunoPlayerMpv::seek_relative(int pos)
{
    qDebug()<<"MainWindow::seek_relative"<<pos;
    if (m_mpv) m_mpv->command(QVariantList() << "seek" << pos << "relative");
}

void XunoPlayerMpv::seekForward()
{
    if (mShifOn){
        showTextOverMovie("seek +1 frame", 1);
        seekFrameForward();
    }else{
    showTextOverMovie("seek +30 sec", 1);
    seek_relative(+30);
    }
}

void XunoPlayerMpv::seekBackward()
{
    if (mShifOn){
        showTextOverMovie("seek -1 frame", 1);
        seekFrameBackward();
    }else{
        showTextOverMovie("seek -10 sec", 1);
        seek_relative(-10);
    }
}

void XunoPlayerMpv::seekFrameForward()
{
    qDebug()<<"MainWindow::seekFrameForward";
    if (m_mpv) m_mpv->command(QVariantList() << "frame-step" );
}

void XunoPlayerMpv::seekFrameBackward()
{
    qDebug()<<"MainWindow::seekFrameBackward";
    if (m_mpv) m_mpv->command(QVariantList() << "frame-back-step" );
}

void XunoPlayerMpv::setMovieSpeed(float speed)
{
    //float rspeed =0.f;
    //if (m_mpv)   rspeed  = m_mpv->getProperty("speed").toFloat();
    qDebug()<<"MainWindow::changeSpeed"<<speed;
    if (speed<0.f){
        //m_mpv->command(QVariantList() << "vf" << "set" << "eq=0.5");
//        m_mpv->command(QVariantList() << "vf" << "set" << "setpts=2*PTS");
//        m_mpv->command(QVariantList() << "af" << "set" << "atempo=0.5,atempo=0.5");
        setMovieColors(1.,-0.5);
    }else if (speed>0.f){
        //m_mpv->command(QVariantList() << "vf" << "set" << "lavfi=[unsharp=7:7:-2:7:7:-2]");
        //m_mpv->command(QVariantList() << "vf" << "set" << "eq=1.0:0.5");
        //m_mpv->command(QVariantList() << "vf" << "set" << "eq=1.5");
        //m_mpv->command(QVariantList() << "vf" << "set" << "setpts=0.5*PTS");
        //m_mpv->command(QVariantList() << "af" << "set" << "atempo=2.0,atempo=2.0");
        //setMovieColors(1.,0.5);
        //m_mpv->command(QVariantList() << "keypress" << "STOP");
        //m_mpv->setProperty("gamma-factor", 2.0);
        //m_mpv->setProperty("sharpen", 2.0);
        m_mpv->setProperty("brightness", 50);
        m_mpv->setProperty("contrast", 50);
    }else if (speed==0.f){
        setMovieColors();
//        m_mpv->command(QVariantList() << "vf" << "set" << "eq=1.0");
//        m_mpv->command(QVariantList() << "vf" << "set" << "setpts=1*PTS");
//        m_mpv->command(QVariantList() << "af" << "set" << "atempo=1.0,atempo=1.0");
    }

}
/**
 * @brief XunuMpvPlayer::setMovieColors
 * https://ffmpeg.org/ffmpeg-filters.html#eq
 * @param contrast      float value in range -2.0 to 2.0. The default value is "1".
 * @param brightness    value in range -1.0 to 1.0. The default value is "0".
 * @param saturation    0.0 to 3.0. The default value is "1".
 * @param gamma         0.1 to 10.0. The default value is "1".
 */
void XunoPlayerMpv::setMovieColors(qreal contrast, qreal brightness, qreal saturation, qreal gamma)
{
    if (m_mpv){
        QString eqfilter=QString("eq=%1:%2:%3:%4").arg(contrast).arg(brightness).arg(saturation).arg(gamma);
        m_mpv->command(QVariantList() << "vf" << "set" << eqfilter);
    }
}

/**
 * @brief XunuMpvPlayer::contrast
 * @param contrast <-100-100> Adjust the contrast of the video signal (default: 0).
 */
void XunoPlayerMpv::contrast(int contrast)
{
    if (m_mpv){
        m_mpv->setProperty("contrast", contrast/4);
    }
}

qreal XunoPlayerMpv::getContrast()
{
    if (m_mpv){
        return m_mpv->getProperty("contrast").toDouble()*4.0;
    }
    return -1.0;
}

/**
 * @brief XunuMpvPlayer::brightness
 * @param brightness=<-100-100> Adjust the brightness of the video signal (default: 0).
 */
void XunoPlayerMpv::brightness(int brightness)
{
    if (m_mpv){
        int rb=brightness/4;
        m_mpv->setProperty("brightness", rb);
    }
}

qreal XunoPlayerMpv::getBrightness()
{
    if (m_mpv){
        return m_mpv->getProperty("brightness").toDouble()*4.0;
    }
    return -1.0;
}

/**
 * @brief XunuMpvPlayer::saturation
 * @param saturation <-100-100> Adjust the saturation of the video signal (default: 0)
 */
void XunoPlayerMpv::saturation(int saturation)
{
    if (m_mpv){
        int rs=saturation/2;
        m_mpv->setProperty("saturation", rs);
    }
}

qreal XunoPlayerMpv::getSaturation()
{
    if (m_mpv){
        return m_mpv->getProperty("saturation").toDouble()*2.0;
    }
    return -1.0;
}

/**
 * @brief XunuMpvPlayer::hue
 * @param hue <-100-100> Adjust the hue of the video signal (default: 0). You can get a colored negative of the image with this option.
 */
void XunoPlayerMpv::hue(int hue)
{
    if (m_mpv){
        m_mpv->setProperty("hue", hue/4);
    }
}

/**
 * @brief XunuMpvPlayer::gamma
 * @param gamma <-100-100> Adjust the gamma of the video signal (default: 0).
 */
void XunoPlayerMpv::gamma(int gamma)
{
      if (m_mpv){
          // qreal fgamma=0.1+(gamma+100)/100.0;
          //m_mpv->setProperty("gamma-factor", fgamma);
          m_mpv->setProperty("gamma", gamma/4);
      }
}
int XunoPlayerMpv::getGamma()
{
      if (m_mpv){
          // qreal fgamma=0.1+(gamma+100)/100.0;
          //m_mpv->setProperty("gamma-factor", fgamma);
          return m_mpv->getProperty("gamma").toInt()*4;
      }
      return -1;
}

/**
 * @brief XunuMpvPlayer::sharpen
 * @param sharpen
 * If set to a value other than 0, enable an unsharp masking filter. Positive values will sharpen the image (but add more ringing and aliasing). Negative values will blur the image.
 */

void XunoPlayerMpv::sharpen(qreal sharpen)
{
    if (m_mpv){
        m_mpv->setProperty("sharpen", sharpen/4);
    }
}

/**
 * @brief XunuMpvPlayer::sharpen
 * @return
 */
qreal XunoPlayerMpv::getSharpen()
{
    if (m_mpv){
        return m_mpv->getProperty("sharpen").toDouble()*4.0;
    }
    return -1.0;
}

void XunoPlayerMpv::onActionEsc()
{
    qDebug()<<"XunuMpvPlayer::onActionEsc()";

    if (m_mpv){
        if (m_mpv->getOsdmsg_used()) {
            showInfo(true);
            return;
        }
    }

    if (isFullScreen()){
        showNormal();
    }else{
        if (mpControl && mpControl->isHidden()){
            tryShowControlBar();
        }else{
            tryHideControlBar();
        }
    }

}

void XunoPlayerMpv::onVideoEQEngineChanged()
{
    qDebug()<<"XunuMpvPlayer::onVideoEQEngineChanged";
    if (mpVideoEQ){
        onBrightnessChanged(mpVideoEQ->brightness()*100);
        onContrastChanged(mpVideoEQ->contrast()*100);
        onHueChanged(mpVideoEQ->hue()*100);
        onSaturationChanged(mpVideoEQ->saturation()*100);
        onGammaRGBChanged(mpVideoEQ->gammaRGB()*100);
        onFilterSharpChanged(mpVideoEQ->filterSharp()*100);
    }

}

void XunoPlayerMpv::onBrightnessChanged(int _brightness)
{
    qDebug()<<"XunuMpvPlayer::onBrightnessChanged"<<_brightness;
    brightness(_brightness);
    if (mpVideoEQ) mpVideoEQ->brightness();
}

void XunoPlayerMpv::onContrastChanged(int _contrast)
{
    qDebug()<<"XunuMpvPlayer::onContrastChanged"<<_contrast;
    contrast(_contrast);
    if (mpVideoEQ) mpVideoEQ->contrast();
}

void XunoPlayerMpv::onHueChanged(int _hue)
{
     qDebug()<<"XunuMpvPlayer::onHueChanged"<<_hue;
     hue(_hue);
     if (mpVideoEQ) mpVideoEQ->hue();
}

void XunoPlayerMpv::onSaturationChanged(int _saturation)
{
    qDebug()<<"XunuMpvPlayer::onSaturationChanged"<<_saturation;
    saturation(_saturation);
    if (mpVideoEQ) mpVideoEQ->saturation();
}

void XunoPlayerMpv::onGammaRGBChanged(int _gamma)
{
    qDebug()<<"XunuMpvPlayer::onGammaRGBChanged"<<_gamma;
    gamma(_gamma);
    if (mpVideoEQ) mpVideoEQ->gammaRGB();

}

void XunoPlayerMpv::onFilterSharpChanged(int _sharp)
{
    qreal s=(_sharp/20.0)+5.0;
    qDebug()<<"XunuMpvPlayer::onFilterSharpChanged"<<s;
    sharpen(s);
    if (mpVideoEQ) mpVideoEQ->filterSharp();

}

void XunoPlayerMpv::pauseResume()
{
    const bool paused = m_mpv->getProperty("pause").toBool();
    const bool idle = m_mpv->getProperty("idle-active").toBool();
    if (mpPlayPauseBtn && !idle){
        mpPlayPauseBtn->setIcon(paused?QIcon(QString::fromLatin1(":/theme/dark/pause.svg")):
                                       QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
         if (m_mpv) m_mpv->setProperty("pause", !paused);
         if (!paused) {
             showTextOverMovie("paused", 3);
         }else{
             showTextOverMovie("play", 1);
         }

    }else{
        mHasPendingPlay=true;
        processPendingActions();
    }
}

void  XunoPlayerMpv::stopUnload(){
     qDebug()<<"MainWindow::stopUnload()";
    if (m_mpv) {
        m_mpv->command(QVariantList() << "stop");
    }
}

void XunoPlayerMpv::setSliderRange(int duration)
{
    //if (m_slider) m_slider->setRange(0, duration);
    if (mpTimeSlider) mpTimeSlider->setRange(0, duration);
    QTime tq= QTime(0, 0, 0);
    tq=tq.addSecs(duration);
    if (mpEnd) mpEnd->setText(tq.toString("hh:mm:ss"));
}

void XunoPlayerMpv::positionChanged(int pos){
    //qDebug()<<"MainWindow::positionChanged"<<pos;
    if (mpTimeSlider) mpTimeSlider->setValue(pos);
    const int tpos = m_mpv->getProperty("time-pos").toInt();
    QTime tq= QTime(0, 0, 0);
    tq=tq.addSecs(tpos);
//    qDebug()<<"MainWindow::positionChanged"<<pos<<tpos<<tq<<tq.toString("hh:mm:ss");
    if (mpCurrent) mpCurrent->setText(tq.toString("hh:mm:ss"));

}

void XunoPlayerMpv::on_START_FILE()
{
     qDebug()<<"MainWindow::on_START_FILE()";
     mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));


}

void XunoPlayerMpv::on_FILE_LOADED()
{
    qDebug()<<"MainWindow::on_FILE_LOADED()";
    const QString movietitle = m_mpv->getProperty("filename").toString();
    mTitle=movietitle;
    this->setWindowTitle(QString("Opened: %1").arg(movietitle));
    mpTimeSlider->setDisabled(false);


    mShowControl = 0;
    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
    ScreenSaver::instance().disable();

    mCursorTimer = startTimer(3000);
    const int duration = m_mpv->getProperty("duration").toInt();
    qDebug()<<"XunuMpvPlayer::on_START_FILE PlayListItem"<<mFile<<mTitle<<duration;


//    PlayListItem item = mpHistory->itemAt(0);
//    item.setUrl(mFile);
//    item.setTitle(mTitle);
//    item.setDuration(duration);
//    mpHistory->setItemAt(item, 0);

    PlayListItem item;
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setLastTime(0);
    mpHistory->remove(mFile);
    mpHistory->insertItemAt(item, 0);

    reSizeByMovie();
}

void XunoPlayerMpv::on_PAUSE()
{
    qDebug()<<"MainWindow::on_PAUSE()";
    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
}

void XunoPlayerMpv::on_END_FILE()
{
    qDebug()<<"MainWindow::on_END_FILE()";
    on_PAUSE();
    this->setWindowTitle(QString("Closed"));
    mpTimeSlider->setValue(0);
    mpTimeSlider->setDisabled(true);


    tryShowControlBar();
    ScreenSaver::instance().enable();
    //toggleRepeat(mpRepeatEnableAction->isChecked());  //after stop not reset repeat task
    //mRepeateMax = 0;
    killTimer(mCursorTimer);
    unsetCursor();
//    if (m_preview)
//        m_preview->setFile(QString());

}

void XunoPlayerMpv::setupUi(QWidget *m_mpv_parent, QWidget *_mpv)
{


//    m_slider = new QSlider();
//    m_slider->setOrientation(Qt::Horizontal);
//    m_openBtn = new QPushButton("Open");
//    m_playBtn = new QPushButton("Pause");
//    QHBoxLayout *hb0 = new QHBoxLayout();
//    hb0->addWidget(m_openBtn);
//    hb0->addWidget(m_playBtn);
//    QVBoxLayout *vl = new QVBoxLayout();
//    vl->addWidget(_mpv);
//    vl->addWidget(m_slider);
//    vl->addLayout(hb0);

    m_mpvLayoutWidget = new QWidget(m_mpv_parent);
    m_mpvLayoutWidget->setContentsMargins(0,0,0,0);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mpvLayoutWidget);
    mainLayout->addWidget(_mpv);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(QMargins());
//    setLayout(mainLayout);

    m_ControlLayoutWidget = new QWidget(m_mpv_parent);
    m_ControlLayoutWidget->setContentsMargins(0,0,0,0);
    //mpPlayerLayout = new QVBoxLayout(m_ControlLayoutWidget);
    m_ControlLayoutWidget->setObjectName("ControlLayoutWidget");

    //m_ControlLayoutWidget->setStyleSheet(QStringLiteral("background-color: rgba(0, 255, 0, 127);"));
    this->setStyleSheet(QStringLiteral("QWidget#ControlLayoutWidget {background-color: rgba(0, 0, 0, 127);}"));

    QString buttons_style_bg= "background-color: rgba(30, 30, 30, 0);";

    QVBoxLayout *mpControlLayout = new QVBoxLayout(m_ControlLayoutWidget);
    mpControlLayout->setSpacing(0);
    mpControlLayout->setMargin(0);
    mpControlLayout->setContentsMargins(0,0,0,0);

    if (Config::instance().floatControlEnabled()){
        detachedControl = new QWidget(m_mpv_parent);
        detachedControl->setWindowTitle(tr("XunoPlayer Controls"));
        detachedControl->setWindowFlags(Qt::Dialog);
        detachedControl->setWindowFlags(detachedControl->windowFlags() & ~Qt::WindowCloseButtonHint);
        detachedControl->setMaximumHeight(125);//785
        detachedControl->setMaximumHeight(55);
        detachedControl->resize(800,detachedControl->minimumHeight());
        this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
        detachedControl->move(this->pos().x(),this->pos().y()+this->rect().height());
        detachedControlLayout = new QVBoxLayout();
        detachedControlLayout->setContentsMargins(0,0,0,0);
        detachedControl->setLayout(detachedControlLayout);
        detachedControl->show();
        detachedControl->raise();
        mpControl = new QWidget(detachedControl);
    }else{
        mpControl = new QWidget(m_ControlLayoutWidget);
        //sent transparent bg
        //mpControl->setStyleSheet(buttons_style_bg);
    }

    //mpControl->setWindowOpacity(0.2);
    mpControl->setMinimumWidth(865);
//    mpControl->setMinimumHeight(25);
//    mpControl->setMaximumHeight(30);


    //mpPreview = new QLable(this);

    mpTimeSlider = new Slider(m_ControlLayoutWidget);


    if (mpTimeSlider){
        mpTimeSlider->setContentsMargins(0,0,0,0);
        mpTimeSlider->setDisabled(true);
        mpTimeSlider->setTracking(true);
        mpTimeSlider->setOrientation(Qt::Horizontal);
        mpTimeSlider->setMinimum(0);
        mpTimeSlider->setStyleSheet(" \
                                    QSlider::groove:horizontal { \
                                        margin: 4px 0px 2px 0px;\
                                        background-color: solid #F8F8F8; \
                                    } \
                                    QSlider::handle:horizontal { \
                                        width: 7px;  \
                                        border: 1px solid #5c5c5c; \
                                        background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4ff./, stop:1 #ffffffff); \
                                        border-radius: 2px;\
                                        margin: -2px 0;\
                                    }\
                                    QSlider::sub-page:horizontal { \
                                        margin: 2px 0px 3px 0px;\
                                        background-color: #0066FF; \
                                    } \
                                    ");

        mpTimeSlider->addVisualLimits();

        mpTimeSlider->setStyleSheet(buttons_style_bg);
    }
    mpCurrent = new QLabel(mpControl);
    mpCurrent->setToolTip(tr("Current time"));
    mpCurrent->setContentsMargins(QMargins(2, 0, 2, 0));
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd = new QLabel(mpControl);
    mpEnd->setToolTip(tr("Duration"));
    mpEnd->setContentsMargins(QMargins(2, 0, 2, 0));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    //    mpTitle = new QLabel(mpControl);
    //    mpTitle->setToolTip(tr("Render engine"));
    //    mpTitle->setText("QPainter");
    //    mpTitle->setIndent(8);
    mpSpeed = new QLabel(QString::fromLatin1("1.00"));
    mpSpeed->setContentsMargins(QMargins(1, 1, 1, 1));
    mpSpeed->setToolTip(tr("Speed. Ctrl+Up/Down"));



    mpPlayPauseBtn = new QToolButton(mpControl);
    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
    mpStopBtn = new QToolButton(mpControl);
    mpStopBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/stop.svg")));
    mpBackwardBtn = new QToolButton(mpControl);
    mpBackwardBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/backward.svg")));
    mpForwardBtn = new QToolButton(mpControl);
    mpForwardBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/forward.svg")));
    mpOpenBtn = new QToolButton(mpControl);
    mpOpenBtn->setToolTip(tr("Open"));
    mpOpenBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/open.svg")));

    mpInfoBtn = new QToolButton(mpControl);
    mpInfoBtn->setToolTip(QString::fromLatin1("Media information"));
    mpInfoBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/info.svg")));
    mpCaptureBtn = new QToolButton(mpControl);
    mpCaptureBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/capture.svg")));
    if (1/*Config::instance().captureType()==Config::CaptureType::DecodedFrame*/)
        mpCaptureBtn->setToolTip(tr("Capture"));
    else
        mpCaptureBtn->setToolTip(tr("Capture Post Filtered"));
    mpVolumeBtn = new QToolButton(mpControl);
    mpVolumeBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/sound.svg")));

    mpVolumeSlider = new Slider(mpControl);
    //mpVolumeSlider->hide();
    mpVolumeSlider->setOrientation(Qt::Horizontal);
    mpVolumeSlider->setMinimum(0);
    const int kVolumeSliderMax = 100;
    mpVolumeSlider->setMaximum(kVolumeSliderMax);
    //mpVolumeSlider->setMaximumHeight(12);
    mpVolumeSlider->setMaximumWidth(88);
    //mpVolumeSlider->setValue(int(1.0/kVolumeInterval*qreal(kVolumeSliderMax)/100.0));
    mpVolumeSlider->setValue(80);
    //setVolume();

    mpWebBtn = new QToolButton(mpControl);
    mpWebBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mpWebBtn->setText(tr("Web"));
    //mpWebBtn->setMaximumHeight(kMaxButtonIconMargin);
    mpWebBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mpWebBtn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpWebBtn->setMinimumHeight(mpInfoBtn->sizeHint().height());
    mpWebBtn->setPopupMode(QToolButton::InstantPopup);
    mpWebBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mpWebBtn->setToolTip(tr("Open XunoWeb browser"));
    mpWebBtn->setStyleSheet(QString::fromLatin1("color:grey;"));


    mpWebMenu = new ConfigWebMemu(mpWebBtn);
    //mpWebMenu->setStyleSheet(buttons_style_bg);
    mpWebBtn->setMenu(mpWebMenu);
    connect(mpWebMenu, SIGNAL(onPlayXunoBrowser(QUrl)), SLOT(onClickXunoBrowser(QUrl)));
    connect(&Config::instance(),SIGNAL(weblinksChanged()),mpWebMenu,SLOT(onChanged()));

    mpFullScreenBtn = new QToolButton(mpControl);
    mpFullScreenBtn->setIcon(QPixmap(QString::fromLatin1(":/theme/dark/fullscreen.svg")));
    //mpFullScreenBtn->setIconSize(QSize(a, a));
    //mpFullScreenBtn->setMaximumSize(a+kMaxButtonIconMargin+2, a+kMaxButtonIconMargin);
    mpFullScreenBtn->setToolTip(tr("Full Screen"));

    mpScaleX15Btn = new QToolButton(mpControl);
    mpScaleX15Btn->setText(tr("x1.5"));
    mpScaleX15Btn->setToolTip(tr("Scale X1.5"));
    mpScaleX15Btn->setStyleSheet(QString::fromLatin1("color:grey;"));
    mpScaleX15Btn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpScaleX15Btn->setMinimumHeight(mpInfoBtn->sizeHint().height());


    mpScaleX2Btn = new QToolButton(mpControl);
    mpScaleX2Btn->setText(tr("x2"));
    mpScaleX2Btn->setToolTip(tr("Scale X2"));
    mpScaleX2Btn->setStyleSheet(QString::fromLatin1("color:grey;"));
    mpScaleX2Btn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpScaleX2Btn->setMinimumHeight(mpInfoBtn->sizeHint().height());

    mpScaleX1Btn = new QToolButton(mpControl);
    mpScaleX1Btn->setText(tr("N"));
    mpScaleX1Btn->setToolTip(tr("Naitive Resolution"));
    mpScaleX1Btn->setStyleSheet(QString::fromLatin1("color:grey;"));
    mpScaleX1Btn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpScaleX1Btn->setMinimumHeight(mpInfoBtn->sizeHint().height());


    mpMenuBtn = new QToolButton(mpControl);
    mpMenuBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/menu.svg")));
    //mpMenuBtn->setAutoRaise(true);
    mpMenuBtn->setPopupMode(QToolButton::InstantPopup);

    mpMenuBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    //mpMenuBtn->setMaximumHeight(a+kMaxButtonIconMargin);
    mpMenuBtn->setText(tr("Menu "));
    mpMenuBtn->setToolTip(tr("Configuratrion menu"));
    mpMenuBtn->setStyleSheet("color:grey;");
    mpMenuBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mpMenuBtn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpMenuBtn->setMinimumHeight(mpInfoBtn->sizeHint().height());

    QMenu *subMenu = 0;
    QWidgetAction *pWA = 0;
    mpMenu = new QMenu();
    mpMenu->addAction(tr("Open File"), this, SLOT(openFile()));
    mpMenu->addAction(tr("Open Url"), this, SLOT(openUrl()));
    //mpMenu->addAction(tr("Online channels"), this, SLOT(onTVMenuClick()));

    //    subMenu = new ClickableMenu(tr("Image Sequence"));
    //mpMenu->addMenu(subMenu);

//    mpImageSequence = new ImageSequenceConfigPage();
//    connect(mpImageSequence, SIGNAL(play(QString)), SLOT(play(QString)));
//    connect(mpImageSequence, SIGNAL(stop()), SLOT(stopUnload()));
//    connect(mpImageSequence, SIGNAL(repeatAChanged(QTime)), SLOT(repeatAChanged(QTime)));
//    connect(mpImageSequence, SIGNAL(repeatBChanged(QTime)), SLOT(repeatBChanged(QTime)));
//    connect(mpImageSequence, SIGNAL(toggleRepeat(bool)), SLOT(toggleRepeat(bool)));
//    connect(mpImageSequence, SIGNAL(customfpsChanged(double)), SLOT(customfpsChanged(double)));
//    connect(mpImageSequence, SIGNAL(toogledFrameExtractor(bool)), SLOT(onImageSequenceToogledFrameExtractor(bool)));
//    connect(mpImageSequence, SIGNAL(setPlayerScale(double)), SLOT(setPlayerScale(double)));
//    connect(mpImageSequence, SIGNAL(RepeatLoopChanged(int)), SLOT(RepeatLoopChanged(int)));

    //mpMenu->addAction(tr("Image Sequence"), this, SLOT(onImageSequenceConfig()));

    //    pWA = new QWidgetAction(0);
    //    pWA->setDefaultWidget(mpImageSequence);
    //    subMenu->addAction(pWA);

    mpMenu->addSeparator();
    subMenu = new QMenu(tr("Play list"));
    mpMenu->addMenu(subMenu);
    mpPlayList = new PlayList(this);
    mpPlayList->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/playlist.qds"));
    mpPlayList->load();
    connect(mpPlayList, SIGNAL(aboutToPlay(QString)),this, SLOT(play(QString)));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpPlayList);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new QMenu(tr("History"));
    mpMenu->addMenu(subMenu);
    mpHistory = new PlayList(this);
    mpHistory->setMaxRows(20);
    mpHistory->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/history.qds"));
    mpHistory->load();
    connect(mpHistory, SIGNAL(aboutToPlay(QString)),this, SLOT(play(QString)));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpHistory);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    mpMenu->addSeparator();

    //mpMenu->addAction(tr("Report"))->setEnabled(false); //report bug, suggestions etc. using maillist?
    //mpMenu->addAction(tr("About"), this, SLOT(about()));
    //mpMenu->addAction(tr("Help"), this, SLOT(help()));
    //mpMenu->addAction(tr("Donate"), this, SLOT(donate()));
    mpMenu->addAction(tr("Setup"), this, SLOT(setup()));
    mpMenu->addSeparator();
    mpMenuBtn->setMenu(mpMenu);
    mpMenu->addSeparator();

    subMenu = new QMenu(tr("Speed"));
    mpMenu->addMenu(subMenu);
    QDoubleSpinBox *pSpeedBox = new QDoubleSpinBox(0);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new ClickableMenu(tr("Repeat"));
    mpMenu->addMenu(subMenu);
    subMenu->setObjectName(QString::fromUtf8("RepeatMenu"));
    //subMenu->setEnabled(false);
    mpRepeatEnableAction = subMenu->addAction(tr("Enable"));
    mpRepeatEnableAction->setCheckable(true);
    //connect(mpRepeatEnableAction, SIGNAL(toggled(bool)), SLOT(toggleRepeat(bool)));
    // TODO: move to a func or class
    mpRepeatLoop = new QCheckBox(tr("Continuous Loop"), this);
    //connect(mpRepeatLoop, SIGNAL(stateChanged(int)), SLOT(RepeatLoopChanged(int)));
    mpRepeatBox = new QSpinBox(0);
    mpRepeatBox->setMinimum(-1);
    mpRepeatBox->setValue(1);
    mpRepeatBox->setToolTip(QString::fromLatin1("-1: ") + tr("infinity"));
    //connect(mpRepeatBox, SIGNAL(valueChanged(int)), SLOT(setRepeateMax(int)));
    QLabel *pRepeatLabel = new QLabel(tr("Times"));
    QHBoxLayout *hb = new QHBoxLayout;
    hb->setObjectName(QString::fromUtf8("TimesLayout"));
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatBox);
    QVBoxLayout *vb = new QVBoxLayout;
    vb->addWidget(mpRepeatLoop);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("From"));
    mpRepeatA = new QTimeEdit();
    mpRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss.zzz"));
    mpRepeatA->setToolTip(tr("negative value means from the end"));
    //connect(mpRepeatA, SIGNAL(timeChanged(QTime)), SLOT(repeatAChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatA);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("To"));
    mpRepeatB = new QTimeEdit();
    mpRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss.zzz"));
    mpRepeatB->setToolTip(tr("negative value means from the end"));
    //connect(mpRepeatB, SIGNAL(timeChanged(QTime)), SLOT(repeatBChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatB);
    vb->addLayout(hb);
    QWidget *wgt = new QWidget;
    wgt->setLayout(vb);

    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    mpRepeatAction = pWA;
    mpRepeatLoop->setCheckState(Qt::Checked);

    mpMenu->addSeparator();

    mpClockMenu = new ClickableMenu(tr("Clock"));
    mpClockMenu->setObjectName(QStringLiteral("Clock"));
    mpMenu->addMenu(mpClockMenu);
    mpClockMenuAction = new QActionGroup(mpClockMenu);
    mpClockMenuAction->setObjectName(QStringLiteral("ClockAction"));
    mpClockMenuAction->setExclusive(true);
    //connect(mpClockMenu, SIGNAL(triggered(QAction*)), SLOT(changeClockType(QAction*)));
    mpClockMenu->addAction(tr("Auto"))->setData(-1);
//    mpClockMenu->addAction(tr("Audio"))->setData(AVClock::AudioClock);
//    mpClockMenu->addAction(tr("Video"))->setData(AVClock::VideoClock);
    foreach(QAction* action, mpClockMenu->actions()) {
        action->setActionGroup(mpClockMenuAction);
        action->setCheckable(true);
    }
    QAction *autoClockAction = mpClockMenu->actions().at(0);
    autoClockAction->setChecked(true);
    autoClockAction->setToolTip(tr("Take effect in next playback"));

    subMenu = new ClickableMenu(tr("Subtitle"));
    mpMenu->addMenu(subMenu);
    QAction *act = subMenu->addAction(tr("Enable"));
    act->setCheckable(true);
//    act->setChecked(mpSubtitle->isEnabled());
    //connect(act, SIGNAL(toggled(bool)), SLOT(toggoleSubtitleEnabled(bool)));
    act = subMenu->addAction(tr("Auto load"));
    act->setCheckable(true);
//    act->setChecked(mpSubtitle->autoLoad());
    //connect(act, SIGNAL(toggled(bool)), SLOT(toggleSubtitleAutoLoad(bool)));
    subMenu->addAction(tr("Open"), this, SLOT(openSubtitle()));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Engine")));
    QComboBox *box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(QString::fromLatin1("FFmpeg"), QString::fromLatin1("FFmpeg"));
    box->addItem(QString::fromLatin1("LibASS"), QString::fromLatin1("LibASS"));
    //connect(box, SIGNAL(activated(QString)), SLOT(setSubtitleEngine(QString)));
//    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
    box->setToolTip(tr("FFmpeg supports more subtitles but only render plain text") + QString::fromLatin1("\n") + tr("LibASS supports ass styles"));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Charset")));
    box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(tr("Auto detect"), QString::fromLatin1("AutoDetect"));
    box->addItem(tr("System"), QString::fromLatin1("System"));
//    foreach (const QByteArray& cs, QTextCodec::availableCodecs()) {
//        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
//    }


    //connect(box, SIGNAL(activated(QString)), SLOT(setSubtitleCharset(QString)));
//    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(tr("Auto detect requires libchardet"));

    subMenu = new ClickableMenu(tr("Audio track"));
    mpMenu->addMenu(subMenu);
    mpAudioTrackMenu = subMenu;
    //connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeAudioTrack(QAction*)));
//    initAudioTrackMenu();

    subMenu = new ClickableMenu(tr("Channel"));
    mpMenu->addMenu(subMenu);
    mpChannelMenu = subMenu;
    //connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeChannel(QAction*)));
//    subMenu->addAction(tr("As input"))->setData(AudioFormat::ChannelLayout_Unsupported); //will set to input in resampler if not supported.
//    subMenu->addAction(tr("Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
//    subMenu->addAction(tr("Mono (center)"))->setData(AudioFormat::ChannelLayout_Center);
//    subMenu->addAction(tr("Left"))->setData(AudioFormat::ChannelLayout_Left);
//    subMenu->addAction(tr("Right"))->setData(AudioFormat::ChannelLayout_Right);
    QActionGroup *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }

    subMenu = new QMenu(tr("Aspect ratio"));
    mpMenu->addMenu(subMenu);
    //connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(switchAspectRatio(QAction*)));
    mpARAction = subMenu->addAction(tr("Video"));
    mpARAction->setData(0);
    subMenu->addAction(tr("Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0/3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0/9.0);
    subMenu->addAction(tr("Custom"))->setData(-2);
    foreach(QAction* action, subMenu->actions()) {
        action->setCheckable(true);
    }
    mpARAction->setChecked(true);

    subMenu = new ClickableMenu(tr("Color space"));
    mpMenu->addMenu(subMenu);
    mpVideoEQ = new VideoEQConfigPage();
    connect(mpVideoEQ, SIGNAL(engineChanged()), SLOT(onVideoEQEngineChanged()));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpVideoEQ);
    subMenu->addAction(pWA);

    mpVideoEQ->setSaveFile(Config::instance().defaultDir() + "/presets.ini");
    mpVideoEQ->loadLocalPresets();

//    subMenu = new ClickableMenu(tr("Decoder"));
//    mpMenu->addMenu(subMenu);
//    mpDecoderConfigPage = new DecoderConfigPage();
//    pWA = new QWidgetAction(0);
//    pWA->setDefaultWidget(mpDecoderConfigPage);
//    subMenu->addAction(pWA);

//    subMenu = new ClickableMenu(tr("Renderer"));
//    mpMenu->addMenu(subMenu);
//    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeVO(QAction*)));
//    //TODO: AVOutput.name,detail(description). check whether it is available
//    VideoRendererId *vo = NULL;
//    while ((vo = VideoRenderer::next(vo))) {
//        // skip non-widget renderers
//        if (*vo == VideoRendererId_OpenGLWindow || *vo == VideoRendererId_GraphicsItem)
//            continue;
//        QAction *voa = subMenu->addAction(QString::fromLatin1(VideoRenderer::name(*vo)));
//        voa->setData(*vo);
//        voa->setCheckable(true);
//        if (!mpVOAction)
//            mpVOAction = voa;
//    }

//    mpVOAction->setChecked(true);
//    mVOActions = subMenu->actions();

    //mpImgSeqExtract->hide();


    //mainLayout->addLayout(mpPlayerLayout);

    if (detachedControlLayout){
        detachedControlLayout->addWidget(mpTimeSlider);
        detachedControlLayout->addWidget(mpControl);
    }else{
        mpControlLayout->setMargin(0);
        mpControlLayout->setContentsMargins(0,0,0,0);
        //mpTimeSlider->setMargin(0);
        mpTimeSlider->setContentsMargins(0,0,0,0);
//        QSlider *qst = new QSlider(Qt::Horizontal,m_ControlLayoutWidget);
//        mpControlLayout->addWidget(qst);
        mpControlLayout->addWidget(mpTimeSlider);
        mpControlLayout->addWidget(mpControl);
        //mpControl->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 127);"));
        //mpTimeSlider->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 127);"));
        //mainLayout->addWidget(mpTimeSlider);
        //mainLayout->addWidget(mpControl);
        m_ControlLayoutWidget->raise();
    }


    // mpControlLayout->addWidget(mpImgSeqExtract);
    //mainLayout->addWidget(mpControl);

    QVBoxLayout *controlVLayout = new QVBoxLayout();
    controlVLayout->setSpacing(0);
    controlVLayout->setMargin(0);
    controlVLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(QMargins(1, 1, 1, 1));

    mpControl->setLayout(controlVLayout);

    controlLayout->addWidget(mpCurrent);
    //controlLayout->addWidget(mpTitle);
    //QSpacerItem *space = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    QSpacerItem *space = new QSpacerItem(1, mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpVolumeSlider);
    controlLayout->addWidget(mpVolumeBtn);
    controlLayout->addWidget(mpCaptureBtn);
    controlLayout->addWidget(mpPlayPauseBtn);
    controlLayout->addWidget(mpStopBtn);
    controlLayout->addWidget(mpBackwardBtn);
    controlLayout->addWidget(mpForwardBtn);
    space = new QSpacerItem(mpPlayPauseBtn->width()/2, mpPlayPauseBtn->height(), QSizePolicy::Fixed);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpWebBtn);
    controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpOpenBtn);
    controlLayout->addWidget(mpInfoBtn);
    //controlLayout->addWidget(mpSpeed);
    //controlLayout->addWidget(mpSetupBtn);
    //controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpFullScreenBtn);
    controlLayout->addWidget(mpScaleX15Btn);
    controlLayout->addWidget(mpScaleX2Btn);
    controlLayout->addWidget(mpScaleX1Btn);

    space = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::Expanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpEnd);

    controlVLayout->setSizeConstraint(QLayout::SetMinimumSize);
//    mpImgSeqExtract = new ImgSeqExtractControl(mpControl);
//    connect(mpImgSeqExtract,SIGNAL(seek(QTime)),SLOT(seek(QTime)));
//    connect(mpImgSeqExtract,SIGNAL(pause()),SLOT(pause()));
//    connect(mpImgSeqExtract,SIGNAL(onStartPlay()),SLOT(onStartPlay()));
//    connect(mpImgSeqExtract,SIGNAL(toggleRepeat(bool)),SLOT(toggleRepeat(bool)));
//    connect(mpImgSeqExtract,SIGNAL(RepeatLoopChanged(int)),SLOT(RepeatLoopChanged(int)));
//    connect(mpImgSeqExtract,SIGNAL(togglePlayPause()),SLOT(togglePlayPause()));
//    connect(mpImgSeqExtract,SIGNAL(repeatAChanged(QTime)),SLOT(repeatAChanged(QTime)));
//    connect(mpImgSeqExtract,SIGNAL(repeatBChanged(QTime)),SLOT(repeatBChanged(QTime)));
//    connect(mpImgSeqExtract,SIGNAL(setTimeSliderVisualMinLimit(QTime)),SLOT(setTimeSliderVisualMinLimit(QTime)));
//    connect(mpImgSeqExtract,SIGNAL(setTimeSliderVisualMaxLimit(QTime)),SLOT(setTimeSliderVisualMaxLimit(QTime)));
//    controlVLayout->addWidget(mpImgSeqExtract);

    controlVLayout->addLayout(controlLayout);

    m_ControlLayoutWidget->setGeometry(0,0,m_mpv->geometry().width(),45);
    //m_ControlLayoutWidget->resize(m_mpv->geometry().width(),m_ControlLayoutWidget->height()+120);


    //m_ControlLayoutWidget->setGeometry(0,0,800,100);

    //setLayout(mainLayout);


    //    mpImgSeqExtract->setVisible(false);
    //    mpImgSeqExtract->setStyleSheet(" \
    //    QSpinBox::up-arrow { \
    //        image: url(:/theme/up_arrow-bw.png); \
    //        width: 7px; \
    //        height: 7px; \
    //    } \
    //    QSpinBox::down-arrow { \
    //        image: url(:/theme/down_arrow-bw.png); \
    //        width: 7px; \
    //        height: 7px; \
    //    } \
    //    ");

    //connect(pSpeedBox, SIGNAL(valueChanged(double)), SLOT(onSpinBoxChanged(double)));
    connect(mpOpenBtn, SIGNAL(clicked()), SLOT(openMedia()));
    connect(mpPlayPauseBtn, SIGNAL(clicked()), SLOT(pauseResume()));
    connect(mpStopBtn, SIGNAL(clicked()), SLOT(stopUnload()));

    connect(mpInfoBtn, SIGNAL(clicked()), SLOT(showInfo()));
    //valueChanged can be triggered by non-mouse event

    connect(mpTimeSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    connect(mpTimeSlider, SIGNAL(sliderPressed()), this,  SLOT(seek()));
    connect(mpTimeSlider, SIGNAL(onLeave()), SLOT(onTimeSliderLeave()));
    connect(mpTimeSlider, SIGNAL(onHover(int,int)), SLOT(onTimeSliderHover(int,int)));


    connect(mpWebBtn, SIGNAL(clicked()), SLOT(onXunoBrowser()));
    connect(mpFullScreenBtn, SIGNAL(clicked()), SLOT(onFullScreen()));

    connect(mpScaleX2Btn, SIGNAL(clicked()), SLOT(onScaleX2Btn()));
    connect(mpScaleX15Btn, SIGNAL(clicked()), SLOT(onScaleX15Btn()));
    connect(mpScaleX1Btn, SIGNAL(clicked()), SLOT(onScaleX1Btn()));



    //connect(m_slider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));

//    connect(m_openBtn, SIGNAL(clicked()), SLOT(openMedia()));
//    connect(m_playBtn, SIGNAL(clicked()), SLOT(pauseResume()));

    connect(mpVolumeBtn, SIGNAL(clicked()), SLOT(setMute()));

    connect(mpVolumeSlider, SIGNAL(sliderPressed()), SLOT(setVolume()));
    connect(mpVolumeSlider, SIGNAL(valueChanged(int)), SLOT(setVolume()));

   // connect(m_mpv_parent, SIGNAL(positionChanged(int)), m_slider, SLOT(setValue(int)));


//    connect(&Config::instance(), SIGNAL(userShaderEnabledChanged()), SLOT(onUserShaderChanged()));
//    connect(&Config::instance(), SIGNAL(intermediateFBOChanged()), SLOT(onUserShaderChanged()));
//    connect(&Config::instance(), SIGNAL(fragHeaderChanged()), SLOT(onUserShaderChanged()));
//    connect(&Config::instance(), SIGNAL(fragSampleChanged()), SLOT(onUserShaderChanged()));
//    connect(&Config::instance(), SIGNAL(fragPostProcessChanged()), SLOT(onUserShaderChanged()));

    QTimer::singleShot(0, this, SLOT(initPlayer()));


}

void XunoPlayerMpv::setMute(){
    if (m_mpv){
      bool isMute=m_mpv->getProperty("ao-mute").toBool();
      if (!isMute) {
          showTextOverMovie("mute", 2);
          //m_mpv->command(QVariantList() << "vf" << "set" << "mirror");
          //m_mpv->command(QVariantList() << "vf" << "set" << "lavfi=[hflip]");
          //m_mpv->command(QVariantList() << "vf" << "set" << "lavfi=[crop=100:100:50:20]");
          //m_mpv->command(QVariantList() << "vf" << "set" << "crop=100:100:50:20");

      }else{
          m_mpv->command(QVariantList() << "vf" << "clr" << "");
          showTextOverMovie(QString("volume %1%").arg(m_mpv->getProperty("ao-volume").toInt()), 2);
      }
      m_mpv->setProperty("ao-mute",!isMute);
      mpVolumeBtn->setChecked(isMute);
    }

}

void XunoPlayerMpv::setVolume()
{
//    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0;
//    //qreal v = qreal(mpVolumeSlider->value())*kVolumeInterval;

//    if (ao) {
//        if (qAbs(int(ao->volume()/kVolumeInterval) - mpVolumeSlider->value()) >= int(0.1/kVolumeInterval)) {
//            ao->setVolume(v);
//        }
//    }

    //qreal v = qreal(mpVolumeSlider->value())/100.;

    int v = mpVolumeSlider->value();


    if (m_mpv) {
        m_mpv->setProperty("ao-volume", v);
        bool isMute=m_mpv->getProperty("ao-mute").toBool();
        if (isMute) {
            showTextOverMovie("mute", 2);
        }else{
            showTextOverMovie(QString("volume %1%").arg(m_mpv->getProperty("ao-volume").toInt()), 1);
        }
    }

    qDebug()<<"MainWindow::setVolume()"<<v;

    mpVolumeSlider->setToolTip(QString::number(v));
    mpVolumeBtn->setToolTip(QString::number(v));



}

void XunoPlayerMpv::onClickXunoBrowser(QUrl url){
    if (url.isValid()) play(url.toString());
}

void XunoPlayerMpv::onFullScreen(){
    if (isFullScreen()){
        showNormal();
    }
    else{
        //showMaximized();
        showFullScreen();
    }
}

void XunoPlayerMpv::onTimeSliderLeave()
{
    /*if (m_preview && m_preview->isVisible())
         m_preview->hide();
         m_preview->hide();*/
    if (!m_preview)
    {
        return;
    }
    if (m_preview->isVisible())
    {
        m_preview->close();
    }
    delete m_preview;
    m_preview = Q_NULLPTR;
}

void XunoPlayerMpv::onTimeSliderHover(int pos, int value)
{
    QPoint gpos;
    if (detachedControl){
        gpos = (detachedControl->pos() + QPoint(pos, 0));
    }else{
        if (m_ControlLayoutWidget){
            gpos = mapToGlobal(mpTimeSlider->pos() + QPoint(pos, m_ControlLayoutWidget->pos().y()-40));
        }
    }
    QToolTip::showText(gpos, QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss.zzz")));
    if (!Config::instance().previewEnabled())
        return;
    if (!m_preview)
        //m_preview = new VideoPreviewWidget();

    if (!m_preview) return;

//    m_preview->setFile(mpPlayer->file());
//    m_preview->setTimestamp(value);
//    m_preview->preview();
    const int w = Config::instance().previewWidth();
    const int h = Config::instance().previewHeight();
    m_preview->setWindowFlags(m_preview->windowFlags() |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_preview->resize(w, h);
    m_preview->move(gpos - QPoint(w/2, h));
    m_preview->show();
}


void XunoPlayerMpv::setup()
{
    ConfigDialog::display();
}


void XunoPlayerMpv::workaroundRendererSize()
{

    if (!m_mpv)
        return;

    QSize s = rect().size();
    //resize(QSize(s.width()-1, s.height()-1));
    //resize(s); //window resize to fullscreen size will create another fullScreenChange event
    m_mpv->resize(QSize(s.width()+1, s.height()+1));
    m_mpv->resize(s);
}


void XunoPlayerMpv::handleFullscreenChange()
{
    workaroundRendererSize();

    // workaround renderer display size for ubuntu
    //tryShowControlBar();
    //QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
}

QSize XunoPlayerMpv::movieSize(){
    QSize t;
    if (m_mpv){
        bool idle = m_mpv->getProperty("idle-active").toBool();
        int w=m_mpv->getProperty("width").toInt();
        int h=m_mpv->getProperty("height").toInt();
        qDebug()<<"MainWindow::movieSize"<<w<<h;
        if (w && h && !idle){
            t=QSize(w,h);
            t*=mPlayerScale;
        }
    }
    qDebug()<<"MainWindow::movieSize t"<<t;
    return t;
}

void XunoPlayerMpv::reSizeByMovie()
{
    if (isFullScreen()) {
        qDebug()<<"skipped, MainWindow::reSizeByMovie(). isFullScreen";
        return;
    }
    QSize t=movieSize();
    if (!t.isEmpty()){
//        if (!Config::instance().floatControlEnabled()){
//            int nh=mpControl->height();
//            qDebug()<<"MainWindow::reSizeByMovie"<<t<<nh;
//            t.setHeight(t.height()+nh);
//        }
        qDebug()<<"MainWindow::reSizeByMovie new "<<t;
        resize(t);
    }


/*
 *
 *
    QSize t=mpRenderer->rendererSize();
    Statistics st=mpPlayer->statistics();

    if (st.video_only.width>0 && st.video_only.height>0 && mPlayerScale>0 ){ //(t.width()+t.height())==0
        t.setWidth(st.video_only.width*mPlayerScale);
        t.setHeight(st.video_only.height*mPlayerScale);
    }else{
        qDebug()<<"skipped, MainWindow::reSizeByMovie(). st.video_only"<<st.video_only.width<<st.video_only.height<<mPlayerScale;
    }
    if (t.isValid() && (!t.isNull())) {
        resize(t);
        if (mpGLSLFilter) {
            mpGLSLFilter->setOutputSize(t);
        }
        //installGLSLFilter(t);
    }else{
       qDebug()<<"skipped, MainWindow::reSizeByMovie(). t.is Not Valid()"<<t;
    }
    //qDebug()<<"reSizeByMovie before calcToUseSuperResolution";
    //calcToUseSuperResolution();
*/
}

void XunoPlayerMpv::onScaleBtn(qreal _scale)
{
    //qDebug()<<"MainWindow: onScaleBtn"<<_scale;
    qreal scale,nextscale15,nextscale20;
    //mPlayerScale

    if (_scale==1.0) {
        nextscale15=1.5;
        nextscale20=2.0;
        scale=1.0;
    }else if (_scale==1.5) {
        scale=mpScaleX15Btn->text().split('x')[1].toFloat();
        nextscale15=(scale==1.)?1.5:1.0;
        nextscale20=2.0;
    }else if (_scale==2.0){
        scale=mpScaleX2Btn->text().split('x')[1].toFloat();
        nextscale15=1.5;
        nextscale20=(scale==1.)?2.0:1.0;
    }
    qDebug()<<"MainWindow: onScaleBtn scale set:"<<scale;

    setPlayerScale(scale);

    mpScaleX15Btn->setText(QString("x%1").arg(nextscale15));
    mpScaleX15Btn->setToolTip(QString("Scale X%1").arg(nextscale15));
    mpScaleX2Btn->setText(QString("x%1").arg(nextscale20));
    mpScaleX2Btn->setToolTip(QString("Scale X%1").arg(nextscale20));
    reSizeByMovie();

    if (scale==2.0){
        qDebug()<<"mpFullScreenBtn Show"<<scale;
        //mpFullScreenBtn->show();
    }else{
        qDebug()<<"mpFullScreenBtn Hide"<<scale;
        //mpFullScreenBtn->hide();
    }

}

void XunoPlayerMpv::onScaleX2Btn()
{
    onScaleBtn(2.0);
}

void XunoPlayerMpv::onScaleX15Btn()
{
    onScaleBtn(1.5);
}

void XunoPlayerMpv::onScaleX1Btn()
{
    onScaleBtn(1.0);
    setPlayerScale(1.0);

//    QSize t=movieSize();
//    if (!t.isEmpty()){
//        resize(t);
//    }


    /*
    if (mpRenderer){
        QSize videoFrame=mpRenderer->videoFrameSize();
        //QSize renderFrame=mpRenderer->rendererSize();
        QSize renderFrame=mpRenderer->videoRect().size();
        if (mPlayerScale>0.) videoFrame*=mPlayerScale;
        //QRectF viewport = QRectF(mpRenderer->mapToFrame(QPointF(0, 0)), mpRenderer->mapToFrame(QPointF(mpRenderer->rendererWidth(), mpRenderer->rendererHeight())));
        qDebug()<<"onScaleX1Btn roi"<<mpRenderer->realROI();
        qDebug()<<"onScaleX1Btn videoFrame"<<videoFrame<<"renderFrame"<<renderFrame<<"maptoFrame"<<QRectF(mpRenderer->mapToFrame(QPointF(0,0)), renderFrame);
        QRectF roi2=QRectF(QPointF(0,0), renderFrame);
        roi2.moveCenter(QPointF(videoFrame.width()/2,videoFrame.height()/2));
        mpRenderer->setRegionOfInterest(roi2);
        mNaitiveScaleOn=true;
    }
    */
}

void XunoPlayerMpv::setPlayerScale(const double scale)
{
    if (scale>0) {
        mPlayerScale=scale;
        mNaitiveScaleOn=false;
        this->setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
        //reset wheelzoom of player
        //emit wheelEvent(new QWheelEvent(QPoint(0,0),0,Qt::NoButton,Qt::NoModifier));
//        if (mpRenderer) {
//            QSize videoFrame=mpRenderer->videoFrameSize();
//            if (mPlayerScale>0.) videoFrame*=mPlayerScale;
//            mpRenderer->setRegionOfInterest(QRectF(QPointF(0,0), videoFrame));
//        }
    }
}


void XunoPlayerMpv::createShortcuts()
{
    QShortcut *quitShortcut = new QShortcut(QKeySequence::Quit, this);
    connect(quitShortcut, &QShortcut::activated, QCoreApplication::quit);

    QShortcut *openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, &QShortcut::activated, this, &XunoPlayerMpv::openMedia);

    QShortcut *toggleShortcut = new QShortcut(Qt::Key_Space, this);
    connect(toggleShortcut, &QShortcut::activated, this, &XunoPlayerMpv::pauseResume);

    QShortcut *playShortcut = new QShortcut(Qt::Key_Play, this);
    connect(playShortcut, &QShortcut::activated, this, &XunoPlayerMpv::pauseResume);

    QShortcut *forwardShortcut = new QShortcut(Qt::Key_Right, this);
    connect(forwardShortcut, &QShortcut::activated, this, &XunoPlayerMpv::seekForward);

    QShortcut *backwardShortcut = new QShortcut(Qt::Key_Left, this);
    connect(backwardShortcut, &QShortcut::activated, this, &XunoPlayerMpv::seekBackward);

    QShortcut *escShortcut = new QShortcut(Qt::Key_Escape, this);
    connect(escShortcut, &QShortcut::activated, this, &XunoPlayerMpv::onActionEsc);


//    QShortcut *increaseShortcut = new QShortcut(Qt::Key_Up, this);
//    connect(increaseShortcut, &QShortcut::activated, volumeButton, &VolumeButton::increaseVolume);

//    QShortcut *decreaseShortcut = new QShortcut(Qt::Key_Down, this);
//    connect(decreaseShortcut, &QShortcut::activated, volumeButton, &VolumeButton::descreaseVolume);
}

void XunoPlayerMpv::updateThumbnailToolBar()
{

}

void XunoPlayerMpv::createThumbnailToolBar()
{
#ifndef unix
    thumbnailToolBar = new QWinThumbnailToolBar(this);
    thumbnailToolBar->setWindow(windowHandle());

    playToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
    playToolButton->setEnabled(false);
    playToolButton->setToolTip(tr("Play"));
    playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playToolButton, &QWinThumbnailToolButton::clicked, this, &XunoPlayerMpv::pauseResume);
#endif

//    forwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
//    forwardToolButton->setEnabled(false);
//    forwardToolButton->setToolTip(tr("Fast forward"));
//    forwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
//    connect(forwardToolButton, &QWinThumbnailToolButton::clicked, this, &MusicPlayer::seekForward);

//    backwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
//    backwardToolButton->setEnabled(false);
//    backwardToolButton->setToolTip(tr("Rewind"));
//    backwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
//    connect(backwardToolButton, &QWinThumbnailToolButton::clicked, this, &MusicPlayer::seekBackward);

//    thumbnailToolBar->addButton(backwardToolButton);
//    thumbnailToolBar->addButton(playToolButton);
//    thumbnailToolBar->addButton(forwardToolButton);

//    connect(&mediaPlayer, &QMediaPlayer::positionChanged, this, &MusicPlayer::updateThumbnailToolBar);
//    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &MusicPlayer::updateThumbnailToolBar);
//    connect(&mediaPlayer, &QMediaPlayer::stateChanged, this, &MusicPlayer::updateThumbnailToolBar);
}


void XunoPlayerMpv::createJumpList()
{
#ifndef unix
    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);
#endif
}
/**
 * @brief XunuMpvPlayer::showTextOverMovie
 * @param txt multine text with Inside quotes, C-style escaping can be used.
 * @param duration in seconds
 */
void XunoPlayerMpv::showTextOverMovie(QString txt, int duration)
{
   if (m_mpv){

       if (!txt.isEmpty()){
           if (txt.size()>15){
               m_mpv->setOption("osd-font-size", "28");
           }else{
               m_mpv->setOption("osd-font-size", "55");
           }
       }
       if (duration){
           m_mpv->setOption("osd-msg1", "");
           m_mpv->setOsdmsg_used(false);
           m_mpv->command(QVariantList() << "show-text" << txt << duration*1000);
       }else{
           if (txt.isEmpty()){
               m_mpv->setOption("osd-msg1", "");
               m_mpv->setOsdmsg_used(false);
           }else{
               if (m_mpv->getOsdmsg_used()){
                   m_mpv->setOsdmsg_used(false);
                   m_mpv->setOption("osd-msg1", "");
               }else{
                   m_mpv->setOsdmsg_used(true);
                   m_mpv->setOption("osd-msg1", txt);
               }
           }
       }
   }
}

void XunoPlayerMpv::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mCursorTimer) {
        if (mpControl->isVisible())
            return;
        setCursor(Qt::BlankCursor);
    }
}

void XunoPlayerMpv::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Control:
        mControlOn=true;
        break;
    case Qt::Key_Shift:
        mShifOn=true;
        break;
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaPause:
    case Qt::Key_Space:
        pauseResume();
        break;
    case Qt::Key_MediaStop:
        stopUnload();
        break;
    case Qt::Key_Left:
        seekForward();
        break;
    case Qt::Key_Right:
        seekBackward();
        break;
    case Qt::Key_Escape:
        onActionEsc();
        break;
    case Qt::Key_Plus:
        if (mControlOn){
            qDebug()<<"Shouldbe:Speed +";
            setMovieSpeed(1);
        }
         break;
    case Qt::Key_Minus:
        if (mControlOn){
            qDebug()<<"Shouldbe:Speed -";
            setMovieSpeed(-1);
        }
         break;
    case Qt::Key_Equal:
        if (mControlOn){
            qDebug()<<"Shouldbe:Speed normal";
            setMovieSpeed(0);
        }
         break;
    case Qt::Key_I:
            qDebug()<<"Show Info";
            showInfo();
         break;
    default:
        qDebug()<<"XunuMpvPlayer::keyPressEvent, unused key pressed:"<<e->key()<<e->text();
        break;
    }


}

void XunoPlayerMpv::keyReleaseEvent(QKeyEvent *e)
{

    if (e->key() == Qt::Key_Control) {
        mControlOn = false;
    }
    if (e->key() == Qt::Key_Shift) {
        mShifOn = false;
    }
}

void XunoPlayerMpv::mousePressEvent(QMouseEvent *e)
{
    //if (!mControlOn)
    //    return;
    mGlobalMouse = e->globalPos();
}

void XunoPlayerMpv::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    mGlobalMouse = QPointF();
}

void XunoPlayerMpv::mouseMoveEvent(QMouseEvent *e)
{
//    unsetCursor();
//    qDebug()<<"XunuMpvPlayer::mouseMoveEvent"<<e->globalPos()<<e->pos();
//    if (e->pos().y() > (height() - 65)) {
//        qDebug()<<"XunuMpvPlayer::mouseMoveEvent height()-65";
//        if (mShowControl == 0) {
//            mShowControl = 1;
//            tryShowControlBar();
//        }
//    } else {
//        if (mShowControl == 1) {
//            mShowControl = 0;
//            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
//        }
//    }


    /*
    if (!mGlobalMouse.isNull()  && (e->buttons() & Qt::LeftButton) ) {
        if (!mpRenderer || !mpRenderer->widget())
            return;
        QRectF roi = mpRenderer->realROI();

        //roi.setTopLeft(roi.topLeft());
        //roi.setSize(roi.size());

        QPointF delta = e->globalPos() - mGlobalMouse;
        mGlobalMouse=e->globalPos();
        QPointF center=roi.center()-delta;
        roi.moveCenter(center);
        if (mNaitiveScaleOn){
            //qDebug()<<"mouseMoveEvent mNaitiveScaleOn";
            //qDebug()<<"mouseMoveEvent roi"<<roi<<"rendererSize"<<mpRenderer->rendererSize();
            QRectF roi2=roi;
//            roi2.setTopLeft(roi2.topLeft()*mPlayerScale);
//            roi2.setSize(roi2.size()*mPlayerScale);

            QSize videoFrame=mpRenderer->videoFrameSize();
            if (mPlayerScale>0.) videoFrame*=mPlayerScale;

            QRectF roi3=QRectF(QPointF(0,0), videoFrame);

            ///qDebug()<<"mouseMoveEvent scaled roi"<<roi<<"rendererSize"<<mpRenderer->rendererSize()<<"roi2"<<roi2<<"roi3"<<roi3<<"r3 intersects r2"<<roi3.intersects(roi2);
            if (roi2.top()>1 && roi2.left()>1 && roi2.right() < roi3.right() && roi2.bottom() < roi3.bottom()){
                mpRenderer->setRegionOfInterest(roi2);
            }
        }else{
            //qDebug()<<"mouseMoveEvent roi"<<roi<<"rendererSize"<<mpRenderer->rendererSize();

            if (roi.top()>1 && roi.left()>1 && roi.right() < mpRenderer->rendererWidth() && roi.bottom() < mpRenderer->rendererHeight()){
                mpRenderer->setRegionOfInterest(roi);
            }
        }
    }
    */
}

void XunoPlayerMpv::wheelEvent(QWheelEvent *e)
{
    /*
    if (!mpRenderer || !mpRenderer->widget()) {
        return;
    }
    QPoint dp;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qreal deg = e->angleDelta().y()/8;
    dp = e->pixelDelta();
#else
    qreal deg = e->delta()/8;
#endif //QT_VERSION
#if WHEEL_SPEED
    if (!mControlOn) {
        qreal speed = mpPlayer->speed();
        mpPlayer->setSpeed(qMax(0.01, speed + deg/15.0*0.02));
        return;
    }
#endif //WHEEL_SPEED
    QSize videoFrame=mpRenderer->videoFrameSize();
    if (mPlayerScale>0.) videoFrame*=mPlayerScale;
    QPointF p = mpRenderer->widget()->mapFrom(this, e->pos());
    QPointF fp = mpRenderer->mapToFrame(p);
    //qDebug() <<  p << fp;
    if (fp.x() < 0)
        fp.setX(0);
    if (fp.y() < 0)
        fp.setY(0);
    if (fp.x() > videoFrame.width())
        fp.setX(videoFrame.width());
    if (fp.y() > videoFrame.height())
        fp.setY(videoFrame.height());
    QRectF viewport = QRectF(mpRenderer->mapToFrame(QPointF(0, 0)), mpRenderer->mapToFrame(QPointF(mpRenderer->rendererWidth(), mpRenderer->rendererHeight())));
    //qDebug("vo: (%.1f, %.1f)=> frame: (%.1f, %.1f)", p.x(), p.y(), fp.x(), fp.y());
    qreal zoom = 1.0 + deg*3.14/180.0;
    if (!dp.isNull()) {
        zoom = 1.0 + (qreal)dp.y()/100.0;
    }
    static qreal z = 1.0;
    z *= zoom;
    if (z < 1.0)
        z = 1.0;
    qreal x0 = fp.x() - fp.x()/z;
    qreal y0 = fp.y() - fp.y()/z;
    //qDebug() << "fr: " << QRectF(x0, y0, qreal(mpRenderer->videoFrameSize().width())/z, qreal(mpRenderer->videoFrameSize().height())/z) << fp << z;
    mpRenderer->setRegionOfInterest(QRectF(x0, y0, qreal(videoFrame.width())/z, qreal(videoFrame.height())/z));
    mNaitiveScaleOn=false;
    return;
    QTransform m;
    m.translate(fp.x(), fp.y());
    m.scale(1.0/zoom, 1.0/zoom);
    m.translate(-fp.x(), -fp.y());
    QRectF r = m.mapRect(mpRenderer->realROI());
    mpRenderer->setRegionOfInterest((r | m.mapRect(viewport))&QRectF(QPointF(0,0), videoFrame));
    */
}

void XunoPlayerMpv::tryHideControlBar()
{
    if (mShowControl > 0) {
        return;
    }
    if ( (mpTimeSlider && mpTimeSlider->isHidden()) &&
         (mpControl && mpControl->isHidden())
         )
        return;

    qDebug()<<"tryHideControlBar";
    if (!detachedControl) mpControl->hide();
    if (mpControl->isHidden()) {
        mpTimeSlider->hide();
        m_ControlLayoutWidget->hide();
    }
    workaroundRendererSize();
//    if (mpRenderer) {
//        qDebug()<<"tryHideControlBar size:"<<mpRenderer->videoFrameSize()<<mpRenderer->rendererSize()<<mpRenderer->widget()->size();
//    }
    //calcToUseSuperResolution();
}

void XunoPlayerMpv::tryShowControlBar()
{
    unsetCursor();
    if ((mpTimeSlider && mpTimeSlider->isHidden()) &&
            (mpControl && mpControl->isHidden())
            ){
        mpTimeSlider->show();
        m_ControlLayoutWidget->show();
    }
    if (detachedControl){
        detachedControl->show();
    }else{
        mpControl->show();
    }


//    if (mpRenderer) {
//        qDebug()<<"MainWindow::tryShowControlBar size:"<<mpRenderer->videoFrameSize()<<mpRenderer->rendererSize()<<mpRenderer->widget()->size();
//    }
    //calcToUseSuperResolution();

}

void XunoPlayerMpv::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    stopUnload();
    qApp->quit();
}

void XunoPlayerMpv::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    QWidget::resizeEvent(e);
    //return;

    //e->ignore();
   // qDebug()<<"MainWindow::resizeEvent"<<e->size()<<e->oldSize();

    calcToUseSuperResolution();
    if (m_mpv && mPlayerScale<=1.5) {
        //QSize framesize=mpRenderer->videoFrameSize();
        QSize framesize=movieSize();
        //qDebug()<<"MainWindow::resizeEvent size:"<<mpRenderer->videoFrameSize()<<mpRenderer->rendererSize()<<mpRenderer->widget()->size();
        if (!framesize.isEmpty()) this->setMaximumSize(framesize*mPlayerScale);
    }else{
        this->setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
    }
    /*
    if (mpTitle)
        QLabelSetElideText(mpTitle, QFileInfo(mFile).fileName(), e->size().width());
    */

    if (m_mpv){
        m_mpvLayoutWidget->resize(this->size());
        //m_mpv->resize(this->size());
        QRect g1=m_mpvLayoutWidget->geometry();
        if (m_ControlLayoutWidget){
            QRect g2=m_ControlLayoutWidget->geometry();
            m_ControlLayoutWidget->setGeometry(0,g1.height()-g2.height(),g1.width(),g2.height());
        }
    }
}

void XunoPlayerMpv::initPlayer()
{

     mIsReady = true;
//     WindowEventFilter *wef = new WindowEventFilter(m_mpv);
//     qApp->installEventFilter(wef);

//    mpFullScreenBtn->hide();
//    mpPlayer = new AVPlayer(this);
//    mIsReady = true;
//    VideoRenderer *vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());
//    if (!vo || !vo->isAvailable() || !vo->widget()) {
//        QMessageBox::critical(0, QString::fromLatin1("QtAV"), tr("Video renderer is ") + tr("not availabe on your platform!"));
//    }
//    setRenderer(vo);
//    //mpSubtitle->installTo(mpPlayer); //filter on frame
//    mpSubtitle->setPlayer(mpPlayer);
//    //mpPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));
//    installGLSLFilter(); //**
//    //installSaveGL();

//    //installSimpleFilter();

//    EventFilter *ef = new EventFilter(mpPlayer);
//    qApp->installEventFilter(ef);
//    ef->setXunoGLSLFilter(mpGLSLFilter);

//    if (mpRenderer && mpRenderer->widget()) {
//        EventRenderer *evR = new EventRenderer(this);
//        mpRenderer->widget()->installEventFilter(evR);
//    }

//    connect(ef, SIGNAL(helpRequested()), SLOT(help()));
//    connect(ef, SIGNAL(showNextOSD()), SLOT(showNextOSD()));
//    onCaptureConfigChanged();
//    onAVFilterVideoConfigChanged();
//    onAVFilterAudioConfigChanged();
//    connect(&Config::instance(), SIGNAL(forceFrameRateChanged()), SLOT(setFrameRate()));
//    connect(&Config::instance(), SIGNAL(captureDirChanged(QString)), SLOT(onCaptureConfigChanged()));
//    connect(&Config::instance(), SIGNAL(captureFormatChanged(QString)), SLOT(onCaptureConfigChanged()));
//    connect(&Config::instance(), SIGNAL(captureQualityChanged(int)), SLOT(onCaptureConfigChanged()));
//    connect(&Config::instance(), SIGNAL(captureTypeChanged(int)), SLOT(onCaptureConfigChanged()));
//    connect(&Config::instance(), SIGNAL(previewEnabledChanged()), SLOT(onPreviewEnabledChanged()));
//    connect(&Config::instance(), SIGNAL(avfilterVideoChanged()), SLOT(onAVFilterVideoConfigChanged()));
//    connect(&Config::instance(), SIGNAL(avfilterAudioChanged()), SLOT(onAVFilterAudioConfigChanged()));
//    connect(&Config::instance(), SIGNAL(bufferValueChanged()), SLOT(onBufferValueChanged()));
//    connect(&Config::instance(), SIGNAL(abortOnTimeoutChanged()), SLOT(onAbortOnTimeoutChanged()));
//    connect(mpStopBtn, SIGNAL(clicked()), this, SLOT(stopUnload()));
    connect(mpForwardBtn, SIGNAL(clicked()), this, SLOT(seekForward()));
    connect(mpBackwardBtn, SIGNAL(clicked()), this, SLOT(seekBackward()));
//    connect(mpVolumeBtn, SIGNAL(clicked()), SLOT(showHideVolumeBar()));
//    connect(mpVolumeSlider, SIGNAL(sliderPressed()), SLOT(setVolume()));
//    connect(mpVolumeSlider, SIGNAL(valueChanged(int)), SLOT(setVolume()));

//    connect(mpPlayer, SIGNAL(seekFinished(qint64)), SLOT(onSeekFinished(qint64)));
//    connect(mpPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), SLOT(onMediaStatusChanged()));
//    connect(mpPlayer, SIGNAL(bufferProgressChanged(qreal)), SLOT(onBufferProgress(qreal)));
//    connect(mpPlayer, SIGNAL(error(QtAV::AVError)), this, SLOT(handleError(QtAV::AVError)));
//    connect(mpPlayer, SIGNAL(started()), this, SLOT(onStartPlay()));
//    connect(mpPlayer, SIGNAL(stopped()), this, SLOT(onStopPlay()));
//    connect(mpPlayer, SIGNAL(paused(bool)), this, SLOT(onPaused(bool)));
//    connect(mpPlayer, SIGNAL(speedChanged(qreal)), this, SLOT(onSpeedChange(qreal)));
//    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChange(qint64)));
//    //connect(mpPlayer, SIGNAL(volumeChanged(qreal)), SLOT(syncVolumeUi(qreal)));

    connect(mpVideoEQ, SIGNAL(brightnessChanged(int)), this, SLOT(onBrightnessChanged(int)));
    connect(mpVideoEQ, SIGNAL(contrastChanged(int)), this, SLOT(onContrastChanged(int)));
    connect(mpVideoEQ, SIGNAL(hueChanegd(int)), this, SLOT(onHueChanged(int)));
    connect(mpVideoEQ, SIGNAL(saturationChanged(int)), this, SLOT(onSaturationChanged(int)));
    connect(mpVideoEQ, SIGNAL(gammaRGBChanged(int)),  this, SLOT(onGammaRGBChanged(int)));
    connect(mpVideoEQ, SIGNAL(filterSharpChanged(int)),  this, SLOT(onFilterSharpChanged(int)));

    emit ready(); //emit this signal after connection. otherwise the slots may not be called for the first time
}


void XunoPlayerMpv::processPendingActions()
{
    if (mHasPendingPlay) {
        mHasPendingPlay = false;
        if (!mFile.isEmpty()){
            this->setWindowTitle(QString("Opening file: %1").arg(mFile));
            showTextOverMovie("replay", 2);
            play(mFile);
        }
    }
}

void XunoPlayerMpv::loadRemoteUrlPresset(const QString& url){
    QString lurl=url;
    if (!lurl.contains("://") || lurl.startsWith("file://")) {
        lurl=QFileInfo(lurl).absolutePath()+"/"+QFileInfo(lurl).baseName()+".config";
    }
    qDebug("XunuMpvPlayer::loadRemoteUrlPresset url: %s, lurl: %s",qPrintable(url),qPrintable(lurl));
    if (lurl.startsWith(XUNOserverUrl,Qt::CaseInsensitive)){
        QString surl=XUNOpresetUrl;
        QByteArray ba;
        ba.append("m="+lurl.remove(XUNOserverUrl+"/",Qt::CaseInsensitive));
        surl.append("q="+ba.toBase64());
        qDebug("MainWindow::openUrl surl: %s",qPrintable(surl));
        mpVideoEQ->setRemoteUrlPresset(surl);
        mpVideoEQ->getRemotePressets();
    }else if (lurl.endsWith(".config")){
        mpVideoEQ->setRemoteUrlPresset(lurl);
        mpVideoEQ->getLocalPressets();
    }
}


void XunoPlayerMpv::calcToUseSuperResolution()
{
   // qDebug()<<"MainWindow::calcToUseSuperResolution";
    if (m_mpv) {

        QSize framesize,rendersize;

        //qDebug()<<"calcToUseSuperResolution: window-scale"<<;
        rendersize=m_mpv->geometry().size();
        framesize=QSize(m_mpv->getProperty("video-params/w").toInt(),m_mpv->getProperty("video-params/h").toInt());



//        if (st.video_only.width>0 && st.video_only.height>0){
//            framesize.setWidth(st.video_only.width);
//            framesize.setHeight(st.video_only.height);
//        }else{
//            framesize=mpRenderer->videoFrameSize();
//        }
//        rendersize=mpRenderer->videoRect().size();//-QSize(1,1);//widget()->size();

        if (framesize.isEmpty()) {
            qDebug()<<"MainWindow::calcToUseSuperResolution framesize.isEmpty";
            return;
        }
        if (rendersize.isEmpty()) {
//            qDebug()<<"MainWindow::calcToUseSuperResolution rendersize.isEmpty()"<<mpRenderer->videoRect().size();
//            return;
            rendersize=framesize;
        }

        qDebug()<<"MainWindow::calcToUseSuperResolution size: framesize:"<<framesize<<"rendersize:"<<rendersize;
        qreal sscaleWidth=qreal(rendersize.width())/qreal(framesize.width());
        qreal sscaleHeight=qreal(rendersize.height())/qreal(framesize.height());
        //qDebug()<<"MainWindow::calcToUseSuperResolution opengl()->video_size"<<mpRenderer
        qreal scale=(sscaleWidth+sscaleHeight)/2.;
        scale=ceil((scale)*100)/100.;
        qDebug()<<"MainWindow::calcToUseSuperResolution Scale WxH:"<<sscaleWidth<<sscaleHeight<<"Middle:"<<scale;

//        needToUseSuperResolutionLastLinearFiltering=true;
//        needToUseSuperResolution=true;

//        if (scale<=1.){
//            needToUseSuperResolution=false;
//        }else if(scale>1.){
//            needToUseSuperResolution=true;
//        }

//        if ((scale>=1.5) || (framesize.width()>=1920.)){
//            needToUseFXAAFiltering=true;
//        }else{
//            needToUseFXAAFiltering=false;
//        }
//        qDebug()<<"needToUseFXAAFiltering:"<<needToUseFXAAFiltering;

////        if (scale==2.){
////            needToUseSuperResolutionLastLinearFiltering=false;
////        }

//        //limit of upper size for frame size more than 1920 pix
//        if (framesize.width()>1920 || framesize.height()>1920) {
//            qDebug()<<"superscale skipped >1920"<<framesize;
//            needToUseSuperResolution=false;
//        }

//        qDebug()<<"needToUseSuperResolution"<<needToUseSuperResolution;
//        qDebug()<<"needToUseSuperResolutionLastLinearFiltering"<<needToUseSuperResolutionLastLinearFiltering;


//        if (mpGLSLFilter!=Q_NULLPTR){
//            mpGLSLFilter->setNeedSuperScale(needToUseSuperResolution);
//            mpGLSLFilter->setNeedSuperScaleLastLinearFiltering(needToUseSuperResolutionLastLinearFiltering);
//            mpGLSLFilter->setNeedToUseFXAAFiltering(needToUseFXAAFiltering);
//        }

        //calculate tunes for XunoSharp values
        const float sharpScaler_x1=0.0001562f;
        const float sharpScaler_x2=0.0002604f;
        const double brightnessScaler=0.0000000;
        const double contrastScaler=0.0000625;
        const double saturationScaler=0.0000312;
        const float gammaScaler=0.0000625f;

        float sharpValue=0.f,gammaValue=0.f;
        double contrastValue=0.,brightnessValue=0.,saturationValue=0.;

        if (scale==1.){
          sharpValue=(framesize.width()*sharpScaler_x1);
        }else if (scale>1.){
            if (scale<2.){
              sharpValue=(framesize.width()*(sharpScaler_x1+sharpScaler_x2*(scale-1.f)));
              gammaValue=framesize.width()*gammaScaler*(scale-1.f);
              contrastValue=framesize.width()*contrastScaler*(scale-1.f);
              brightnessValue=framesize.width()*brightnessScaler*(scale-1.f);
              saturationValue=framesize.width()*saturationScaler*(scale-1.f);
            }else{
              sharpValue=(framesize.width()*(sharpScaler_x1+sharpScaler_x2));
              gammaValue=framesize.width()*gammaScaler;
              contrastValue=framesize.width()*contrastScaler;
              brightnessValue=framesize.width()*brightnessScaler;
              saturationValue=framesize.width()*saturationScaler;
            }
        }

        if (m_mpv) {
            if (1) {
                qreal userValue=getSharpen();   // mpVideoEQ->filterSharp();
                qreal correctedValue=userValue+sharpValue;
                sharpen(correctedValue);
                qDebug()<<"Set XunoSharp Value (corrected,user,correction)"<<correctedValue<<userValue<<sharpValue;
            }
            if (gammaValue>=0.f) {
                qreal userValue=qreal(getGamma());
                qreal correctedValue=userValue+gammaValue;
                gamma(int(correctedValue));
                qDebug()<<"Set XunoGamma Value (corrected,user,correction)"<<correctedValue<<userValue<<gammaValue;
            }
        }

        if (m_mpv) {//mpGLSLFilter
            if (contrastValue>=0.) {
                qreal userValue=getContrast();
                qreal correctedValue=userValue+contrastValue;
                contrast(correctedValue);
                qDebug()<<"Set XunoContrast Value (corrected,user,correction)"<<correctedValue<<userValue<<contrastValue;
            }
            if (brightnessValue>=0.) {
                qreal userValue=getBrightness();
                qreal correctedValue=userValue+brightnessValue;
                brightness(correctedValue);
                qDebug()<<"Set XunoBrightness Value (corrected,user,correction)"<<correctedValue<<userValue<<brightnessValue;
            }
            if (saturationValue>=0.) {
                qreal userValue=getSaturation();
                qreal correctedValue=userValue+saturationValue;
                saturation(correctedValue);
                qDebug()<<"Set XunoSaturation Value (corrected,user,correction)"<<correctedValue<<userValue<<saturationValue;
            }
        }
    }
}

bool XunoPlayerMpv::showInfo(bool hide)
{
    if (m_mpv)  {

        //m_mpv->command("keypress display-stats");
        qDebug()<<"XunuMpvPlayer::showInfo()"<<"m_mpv->command(\"keypress display-stats\")";
        //QVariant propList1=m_mpv->getProperty("osd-ass-cc/0");
        //qDebug()<<"osd-ass-cc/0"<<propList1;
        qreal fps=m_mpv->current_fps();
        QSize rendersize=m_mpv->geometry().size();
        QString xunoversion;
#ifdef VERSION_STRING
#ifdef VERGIT
        xunoversion=QString("%1-git:%2").arg(VERSION_STRING).arg(VERGIT);
#endif
#endif
        QString t=QString("Filename : ${filename}\n"
                  "Size : ${file-size}, Format : ${file-format}, Demuxer : ${current-demuxer}\n"
                  "Duration : ${duration}, Pos : ${time-pos}\n"
                  "Video format : ${video-format}, Video Codec : ${video-codec}\n"
                  "Pixel format : ${video-params/pixelformat}\n"
                  "Average BPP : ${video-params/average-bpp}\n"
                  "Video size : ${width}x${height}\n"
                  "Display size : %2x%3\n"
                  "hwdec current : ${hwdec-current}\n"
                  "hwdec interop : ${hwdec-interop}\n"
                  "hwdec : ${hwdec}\n"
                  "Video fps : ${container-fps}, estimated fps : ${estimated-vf-fps}, display fps : %1\n"
                  "Video aspect : ${video-aspect}\n"
                  "Video bitrate : ${video-bitrate}\n"
                  "Audio bitrate : ${audio-bitrate}\n"
                  "mpv version : ${mpv-version}\n"
                  "ffmpeg version : ${ffmpeg-version}\n"
                  "XunoPlayer-MPV version : %4\n"
                   ).arg(fps).arg(rendersize.width()).arg(rendersize.height()).arg(xunoversion);
                          ;


        if (m_mpv){
            if (!hide){
                QVariant etext=m_mpv->command_result(QVariantList() << "expand-text" << t );
                showTextOverMovie(etext.toString(),0);
            }else{
                showTextOverMovie("",0);
            }
            // m_mpv->command(QVariantList() << "overlay-add" << 0 << 0 << 0 << "./XunoPlayer-MPV_300x300.BGRA" << 0 << "bgra" << 150 << 150<< 1200 );
        }

        return m_mpv->getOsdmsg_used();

       //m_mpv->command(QVariantList()<<"load-script"<<"./stats.lua");
        //m_mpv->setProperty("input-conf", "lex.conf");
        //mpv_set_option_string(mpv, "input-conf", "./lex.conf");
        QVariant propList=m_mpv->getProperty("property-list");
        QVariantList vl=propList.toList();
      // qDebug()<<"property-list:"<<vl;

        foreach (QVariant li, vl)
        {
            QString s0=li.toString();
            if (s0.contains("options")) continue;
            if (s0.contains("property-list")) continue;
            QVariant vfmetadata=m_mpv->getProperty(s0);
            QVariantList vl1=vfmetadata.toList();


            if (vl1.isEmpty()){
                qDebug()<<"property-list:"<<li.toString()<<"value:"<<vfmetadata;
//                QVariantMap  vm=vfmetadata.toMap();
//                if (!vm.isEmpty()) {
//                    qDebug()<<"property-list:"<<li.toString();
//                    foreach (QVariant vmi, vm) {
//                        QString s=vmi.toString();
//                        if (!s.isEmpty())  qDebug()<<"  map    :"<<s<<vm.value(s);
//                    }
//                }else{
//                    qDebug()<<"property-list:"<<li.toString()<<"value:"<<vfmetadata;
//                }



            }else{
                qDebug()<<"property-list:"<<li.toString();
                foreach (QVariant li2, vl1) {
                    QString s=li2.toString();
                    if (!s.isEmpty())  qDebug()<<"      :"<<s;
                }
            }
        }

//       QVariant vfmetadata=m_mpv->getProperty("metadata");
//       qDebug()<<"vfmetadata:"<<vfmetadata;

//       m_mpv->command(QVariantList()<<"enable-section"<<"hd");
//       m_mpv->command(QVariantList()<<"keypress"<<"q");
        //m_mpv->command(QVariantList()<<"show-progress");
        //m_mpv->command("show-progress");
    }

    //mpStatisticsView->show();
    return true;
}
