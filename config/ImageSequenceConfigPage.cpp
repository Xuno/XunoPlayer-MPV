#include "ImageSequenceConfigPage.h"


ImageSequenceConfigPage::ImageSequenceConfigPage(QWidget *parent) :
    QDialog(parent)
  ,startPos(0)
  ,stopPos(0)
  ,fps(25)
  ,startFrame(0)
  ,frames(0)
  ,fileinfo(0)
{
    setWindowTitle("Play Image Sequence");
    setMaximumSize(QSize(650, 16777215));

    QVBoxLayout *verticalLayoutWidget = new QVBoxLayout(this);
    setLayout(verticalLayoutWidget);

    QGroupBox *groupBox1 = new QGroupBox(this);
    groupBox1->setObjectName(QStringLiteral("groupBox1"));
    groupBox1->setTitle(tr("Play Image Sequence    i.e.").append(" c:\\raw\\*.tif"));

    QVBoxLayout *vb = new QVBoxLayout(groupBox1);

    vb->setSpacing(6);
    vb->setContentsMargins(11, 11, 11, 11);

    InputPath = new QLineEdit();
    InputPath->setObjectName(QStringLiteral("InputPath"));
    vb->addWidget(InputPath);

    QHBoxLayout *horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
    QPushButton *btSelectInputPath = new QPushButton();
    btSelectInputPath->setObjectName(QStringLiteral("btSelectInputPath"));
    btSelectInputPath->setText(tr("Select File Range", 0));
    horizontalLayout_2->addWidget(btSelectInputPath);
    connect(btSelectInputPath, SIGNAL(clicked()), SLOT(onSelectImgages()));

    QSpacerItem *horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer_3);

    QLabel *pFPSLabel = new QLabel(tr("FPS:"));

    mpFpsBox = new QDoubleSpinBox(0);
    mpFpsBox->setMinimum(0.1);
    mpFpsBox->setMaximum(500);
    mpFpsBox->setValue(25);
    mpFpsBox->setSingleStep(1);
    mpFpsBox->setToolTip("frames per second.");
    connect(mpFpsBox, SIGNAL(valueChanged(double)), SLOT(setFPS(double)));
    horizontalLayout_2->addWidget(pFPSLabel);
    horizontalLayout_2->addWidget(mpFpsBox);

    QSpacerItem *horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_4);

    QLabel *label_5 = new QLabel();
    label_5->setText(tr("Color Depth:", 0));
    label_5->setObjectName(QStringLiteral("label_5"));

    horizontalLayout_2->addWidget(label_5);

    cbColorTypeInput = new QComboBox();
    cbColorTypeInput->setObjectName(QStringLiteral("cbColorTypeInput"));
    cbColorTypeInput->clear();
    cbColorTypeInput->insertItems(0, QStringList()
                                  << tr("8-bit RGB", 0)
                                  //     << tr("10-bit RGB", 0)
                                  //     << tr("12-bit RGB", 0)
                                  //     << tr("14-bit RGB", 0)
                                  //     << tr("16-bit RGB", 0)
                                  );
    horizontalLayout_2->addWidget(cbColorTypeInput);

    QSpacerItem *horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_7);

    QLabel *labelInputScale = new QLabel();
    labelInputScale->setObjectName(QStringLiteral("InputScale"));
    labelInputScale->setText(tr(" Scale:"));
    InputScale = new QDoubleSpinBox();
    InputScale->setMinimum(0.1);
    InputScale->setMaximum(2.0);
    InputScale->setSingleStep(0.1);
    InputScale->setValue(1.0);

    horizontalLayout_2->addWidget(labelInputScale);
    horizontalLayout_2->addWidget(InputScale);

    vb->addLayout(horizontalLayout_2);

    QHBoxLayout *hlStart = new QHBoxLayout();
    hlStart->setSpacing(6);
    hlStart->setObjectName(QStringLiteral("hlStart"));
    QLabel *label_2 = new QLabel();
    label_2->setObjectName(QStringLiteral("label_2"));
    label_2->setText(tr("Start Frame:", 0));

    hlStart->addWidget(label_2);

    InputStartFrame = new QSpinBox();
    InputStartFrame->setObjectName(QStringLiteral("InputStartFrame"));
    InputStartFrame->setEnabled(false);
    InputStartFrame->setMinimum(1);
    InputStartFrame->setMaximum(999999999);
    InputStartFrame->setValue(1);

    hlStart->addWidget(InputStartFrame);

    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart->addItem(horizontalSpacer_2);

    QLabel *label_28 = new QLabel();
    label_28->setObjectName(QStringLiteral("label_28"));
    label_28->setText(tr("End Frame:", 0));

    hlStart->addWidget(label_28);

    InputEndFrame = new QSpinBox();
    InputEndFrame->setObjectName(QStringLiteral("InputEndFrame"));
    InputEndFrame->setEnabled(false);
    InputEndFrame->setMinimum(1);
    InputEndFrame->setMaximum(999999999);
    InputEndFrame->setValue(1);

    hlStart->addWidget(InputEndFrame);

    QSpacerItem *horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart->addItem(horizontalSpacer_5);

    QLabel *label_4 = new QLabel();
    label_4->setObjectName(QStringLiteral("label_4"));
    label_4->setText(tr("Total Frames:", 0));
    hlStart->addWidget(label_4);

    InputTotalFrame = new QSpinBox();
    InputTotalFrame->setObjectName(QStringLiteral("InputTotalFrame"));
    InputTotalFrame->setEnabled(false);
    InputTotalFrame->setMinimum(1);
    InputTotalFrame->setMaximum(999999999);
    InputTotalFrame->setValue(1);

    hlStart->addWidget(InputTotalFrame);

    InputAllTotalFrame = new QLabel();
    InputAllTotalFrame->setObjectName(QStringLiteral("InputAllTotalFrame"));
    InputAllTotalFrame->setText(tr("of: NNNNN", 0));
    hlStart->addWidget(InputAllTotalFrame);

    QSpacerItem *horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlStart->addItem(horizontalSpacer_6);

    checkLoop = new QCheckBox();
    checkLoop->setObjectName(QStringLiteral("checkLoop"));
    checkLoop->setText(tr("Loop"));
    hlStart->addWidget(checkLoop);

    vb->addLayout(hlStart);

    QHBoxLayout *horizontalLayout_11 = new QHBoxLayout();
    horizontalLayout_11->setSpacing(6);
    horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
    cbDecodeGeometryFromFileName = new QCheckBox();
    cbDecodeGeometryFromFileName->setObjectName(QStringLiteral("cbDecodeGeometryFromFileName"));
    cbDecodeGeometryFromFileName->setChecked(true);

    horizontalLayout_11->addWidget(cbDecodeGeometryFromFileName);

    QSpacerItem *horizontalSpacer_16 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_11->addItem(horizontalSpacer_16);

    QLabel *label_input_w = new QLabel();
    label_input_w->setObjectName(QStringLiteral("label_input_w"));

    horizontalLayout_11->addWidget(label_input_w);

    inputImageW = new QSpinBox();
    inputImageW->setObjectName(QStringLiteral("inputImageW"));
    inputImageW->setMaximum(32768);

    horizontalLayout_11->addWidget(inputImageW);

    QLabel *label_input_h = new QLabel();
    label_input_h->setObjectName(QStringLiteral("label_input_h"));

    horizontalLayout_11->addWidget(label_input_h);

    inputImageH = new QSpinBox();
    inputImageH->setObjectName(QStringLiteral("inputImageH"));
    inputImageH->setMaximum(32768);

    horizontalLayout_11->addWidget(inputImageH);

    vb->addLayout(horizontalLayout_11);
#ifndef QT_NO_TOOLTIP
    cbDecodeGeometryFromFileName->setToolTip(tr( "Example of filename: image_5216x3472_8bit.RGB"));
#endif // QT_NO_TOOLTIP
    cbDecodeGeometryFromFileName->setText(tr( "Decode geometry of image from filename"));
    label_input_w->setText(tr( "Width:"));
    label_input_h->setText(tr( "Height:"));

    verticalLayoutWidget->addWidget(groupBox1);

    QGroupBox *groupBox2 = new QGroupBox(this);
    groupBox2->setObjectName(QStringLiteral("groupBox2"));
    groupBox2->setTitle(tr("Generate Image Sequence"));

    QVBoxLayout *vb2 = new QVBoxLayout(groupBox2);
    const QString buttTextPadding="padding: 3px 7px 4px 7px;";
    checkBoxExtractor = new QPushButton(groupBox2);
    checkBoxExtractor->setObjectName(QStringLiteral("checkBoxExtractor"));
    checkBoxExtractor->setStyleSheet(buttTextPadding);
    checkBoxExtractor->setText(tr("Enable Frame Extractor"));
    checkBoxExtractor->setCheckable(true);
    checkBoxExtractor->setChecked(false);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(checkBoxExtractor->sizePolicy().hasHeightForWidth());
    checkBoxExtractor->setSizePolicy(sizePolicy);
    vb2->addWidget(checkBoxExtractor);

    groupBox2->hide();
    verticalLayoutWidget->addWidget(groupBox2);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    buttonBox->setCenterButtons(true);
    buttonPlay = buttonBox->addButton(QObject::tr("Play"), QDialogButtonBox::AcceptRole);
    buttonPlay->setDisabled(InputPath->text().isEmpty());
    connect(buttonBox,SIGNAL(rejected()),SLOT(close()));
    connect(buttonBox,SIGNAL(accepted()), SLOT(playImgages()));


    verticalLayoutWidget->addWidget(buttonBox);

    //    QLabel *lablelGenerate = new QLabel(tr("Generate Image Sequence"));
    //    vb->addWidget(lablelGenerate);

    QMetaObject::connectSlotsByName(this);

    //initValues

    setEnableFrameExtractor(false);
}

void ImageSequenceConfigPage::onSelectImgages()
{
    //build filter list for FileDialog Menu
    QString type = tr("All images").append(" (");
    for (int i = 0; i < ImageTypes.size(); ++i){
        if (i>0) type.append(" ");
        type.append(QString("*.").append(ImageTypes.at(i)));
    }
    type.append(")");
    for (int i = 0; i < ImageTypes.size(); ++i){
        if (i!=ImageTypes.size()) type.append(";;");
        type.append(QString("*.").append(ImageTypes.at(i)));
    }
    //get latest used path
    //QString prevPath="E:\\public\\Videos\\OWN\\work\\image-seq\\outbmp-bgr24";//= config.getLastUsedPath();
    QString prevPath=InputPath->text();
    QStringList fileNameList = QFileDialog::getOpenFileNames(this,tr("Open Input"), prevPath, type);
    if (fileNameList.size()>0){
        QString fileName=QDir::toNativeSeparators(fileNameList.at(0));
        if (!fileName.isEmpty())
        {
            InputPath->setText(getSequenceFilename(fileName));
            buttonPlay->setDisabled(InputPath->text().isEmpty());
            //config.setLastUsedPath(fileName);
            int startframe=getNumberFilename(fileName);
            int lastframe=getNumberFilename(fileNameList.last());
            int totalframes=lastframe-startframe+1;
            totalAllInputFrames=getTotalNumberFilename(fileName);
            InputStartFrame->setMaximum(totalAllInputFrames);
            InputEndFrame->setMaximum(totalAllInputFrames);
            InputTotalFrame->setMaximum(totalAllInputFrames);
            if (startframe>0) {
                setStartFrame(startframe);
                InputAllTotalFrame->setText(tr(" of: %1").arg(totalAllInputFrames));
            }
            if (lastframe>0 && totalframes>1) {
                InputEndFrame->setEnabled(true);
                InputEndFrame->setValue(lastframe);
            }
            if (totalframes>1) {
                setTotalFrames(totalframes);
                InputTotalFrame->setEnabled(true);
            }else if(totalframes==1){
                setTotalFrames(totalAllInputFrames);
                InputTotalFrame->setEnabled(true);
                InputTotalFrame->setValue(totalAllInputFrames);
            }
        }
    }

    calculatePos();

    //---------
    //    QStringList files = QFileDialog::getOpenFileNames(parentWidget(), tr("Select one or more media file"),"",tr("Image Files (*.png *.jpg *.tif *.bmp)"));
    //    if (files.isEmpty())
    //        return;
    //    if (files.size()>1) mpTotalFramesBox->setValue(files.size());
    //    fileinfo = QFileInfo(files.at(0));
    //    analyzeFilename();

    //    playImgages();
    //    return;
}

void ImageSequenceConfigPage::analyzeFilename(){
    bool ok = false;
    QString f=fileinfo.baseName();
    qint32 fr=f.toLong(&ok,10);
    if (fr>0 && ok) startFrame=fr;
    qDebug()<<"analyzeFilename :: startFrame :"<<startFrame;
}

int ImageSequenceConfigPage::getDigetsFilename() {
    int i=0;
    QRegularExpression re("(\\d+)$");
    QRegularExpressionMatch match = re.match(fileinfo.baseName());
    if (match.hasMatch()) {
        i=match.captured(0).count();
    }
    return i;
}


void ImageSequenceConfigPage::playImgages()
{
    calculatePos();
    //    int digs=getDigetsFilename();
    //    //qDebug()<<"playImgages :: getDigetsFilename :"<<digs;
    //    QString filename=fileinfo.absolutePath().append("/%0").append(QString("%1d.").arg(digs)).append(fileinfo.suffix());
    //    //qDebug()<<"playImgages :: filename :"<<filename;
    //    if (digs==0) return;
    emit customfpsChanged(fps);
    if (InputScale->value()) emit setPlayerScale(InputScale->value());
    emit RepeatLoopChanged(checkLoop->isChecked()?Qt::Checked:Qt::Unchecked);
    //    //if (startPos) emit toggleRepeat(true);
    //    playing_start=true;
    emit stop();
    QString imageseqprefix="mf://";
    emit play(imageseqprefix.append(InputPath->text()));
    qDebug()<<"playImgages :: filename :";
    // emit finished(QDialog::Accepted);
    //emit accepted();
    //setResult (QDialog::Accepted);
    //hide();
    accept();
}

void ImageSequenceConfigPage::setFPS(double n){
    fps = n;
    if (n) emit stop();
    double step;
    if (fps>1){
        step=1;
    }else if (fps<=1){
        step=0.1;
    }
    mpFpsBox->setSingleStep(step);
    if (fps==1.1) fps=2;
    else if (fps>1.1) fps=(double)qRound(fps);
    mpFpsBox->setValue(fps);
    emit customfpsChanged(fps);
    calculatePos();
}



void ImageSequenceConfigPage::setStartFrame(quint32 n){
    startFrame = n;
    if (startFrame) {
        InputStartFrame->setValue(startFrame);
        InputStartFrame->setEnabled(true);
        emit toggleRepeat(true);
        calculatePos();
    }
}

void ImageSequenceConfigPage::setEndFrame(quint32 n){
    qDebug()<<"ImageSequenceConfigPage::setEndFrame"<<n<<startFrame;
    if (n){
        InputEndFrame->setValue(n);
        InputEndFrame->setEnabled(true);
        InputTotalFrame->setValue(n-startFrame+1UL);
        setTotalFrames(n-startFrame+1);
    }
}

void ImageSequenceConfigPage::setTotalFrames(int n){
    //qDebug()<<"ImageSequenceConfigPage::setTotalFrames"<<n;
    frames = n;
    calculatePos();
}

void ImageSequenceConfigPage::setImageSequenceFileName(QString fname){
    QString fileName=fname;
    if (!fileName.isEmpty())
    {
        InputPath->setText(fileName);
        buttonPlay->setDisabled(InputPath->text().isEmpty());
        totalAllInputFrames=getTotalNumberFilename(fileName);
        //qDebug()<<"setImageSequenceFileName"<<fileName<<totalAllInputFrames;
        InputStartFrame->setMaximum(totalAllInputFrames);
        InputEndFrame->setMaximum(totalAllInputFrames);
        InputTotalFrame->setMaximum(totalAllInputFrames);
        InputAllTotalFrame->setText(tr(" of: %1").arg(totalAllInputFrames));
    }
}

void ImageSequenceConfigPage::calculatePos(){
    startFrame=InputStartFrame->value();
    frames=InputTotalFrame->value();

    qDebug()<<"calculatePos frames"<<frames;
    if (frames<1 || fps<1)
        return;
    startPos=(startFrame-1UL)*(1000UL/fps);
    stopPos=startPos + frames*(1000UL/fps);
    QTime mRepeatA = QTime(0, 0, 0).addMSecs(startPos);
    QTime mRepeatB = QTime(0, 0, 0).addMSecs(stopPos);
    if (startPos) emit repeatAChanged(mRepeatA);
    if (stopPos) emit repeatBChanged(mRepeatB);
    qDebug()<<"calculatePos mRepeatA:"<<mRepeatA<<",mRepeatB:"<<mRepeatB<<"startPos"<<startPos<<"stopPos"<<stopPos;
}

void ImageSequenceConfigPage::setMovieDuration(qint64 d){
    if (playing_start) {
        QTime mRepeatB = QTime(0, 0, 0).addMSecs(d);
        if (stopPos==0) emit repeatBChanged(mRepeatB);
        playing_start=false;
    }
}

void ImageSequenceConfigPage::on_checkBoxExtractor_toggled(bool state)
{
    qDebug()<<"on_checkBoxExtractor_toggled"<<state;
    QPushButton* button = dynamic_cast<QPushButton*>(sender());
    QString text=button->text();
    const QString enable=tr("Enable");
    //const QString enabled=tr("Enabled");
    const QString disable=tr("Disable");
    if (state)
        text.replace(enable,disable);
    else
        text.replace(disable,enable);
    button->setText(text);
    emit toogledFrameExtractor(state);
}

void ImageSequenceConfigPage::on_checkLoop_toggled(bool state)
{
    qDebug()<<"on_checkLoop_toggled"<<state;
}

int ImageSequenceConfigPage::getDigetsFilename(QString filename) {
    QFileInfo fileinfo = QFileInfo(filename);
    int i=0;
    QRegularExpression re("(\\d+)$");
    QRegularExpressionMatch match = re.match(fileinfo.baseName());
    if (match.hasMatch()) {
        i=match.captured(0).count();
    }
    return i;
}

int ImageSequenceConfigPage::getNumberFilename(QString filename) {
    QFileInfo fileinfo = QFileInfo(filename);
    QDir dirinfo=fileinfo.absoluteDir();
    fileinfo.completeSuffix();
    QStringList filelist = dirinfo.entryList(QStringList()<<QString("*").append(fileinfo.completeSuffix()),QDir::Files,QDir::Name);
    int i=filelist.indexOf(fileinfo.fileName())+1;
    return i;
}

int ImageSequenceConfigPage::getTotalNumberFilename(QString filename) {
    QFileInfo fileinfo = QFileInfo(filename);
    QDir dirinfo=fileinfo.absoluteDir();
    fileinfo.completeSuffix();
    QStringList filelist = dirinfo.entryList(QStringList()<<QString("*").append(fileinfo.completeSuffix()),QDir::Files,QDir::Name);
    return filelist.size();
}


QString ImageSequenceConfigPage::getSequenceFilename(QString filename) {
    QFileInfo fileinfo = QFileInfo(filename);
    if (fileinfo.completeBaseName().contains('*')) return filename;
    int digs=getDigetsFilename(filename);
    QString prefix="";
    prefix=fileinfo.completeBaseName();
    prefix.chop(digs);
    //QString newfilename=QDir::toNativeSeparators(fileinfo.absolutePath()).append(QDir::separator()).append(prefix).append("%0").append(QString("%1d.").arg(digs)).append(fileinfo.suffix());
    QString newfilename=QDir::toNativeSeparators(fileinfo.absolutePath()).append(QDir::separator()).append(prefix).append("*").append(fileinfo.suffix());
    return (newfilename);
}


void ImageSequenceConfigPage::updateInputTotalFrameValue(){
    qint32 start=InputStartFrame->value();
    qint32 end=InputEndFrame->value();
    if (start>0 && end>0){
        qint32 total=end-start+1;
        InputTotalFrame->setEnabled(total>0);
        if (total) {
            InputTotalFrame->setValue(total);
            //qDebug()<<"updateInputTotalFrameValue"<<totalAllInputFrames<<start;
            //InputTotalFrame->setMaximum(totalAllInputFrames-start+1);
        }
    }
}

void ImageSequenceConfigPage::updateInputEndFrameValue(){
    qint32 start=InputStartFrame->value();
    qint32 total=InputTotalFrame->value();
    qint32 end=start+total-1;
    InputEndFrame->setEnabled(end>0);
    if (end) {
        InputEndFrame->setValue(end);
    }
}


void ImageSequenceConfigPage::on_InputStartFrame_valueChanged(int arg1)
{
    Q_UNUSED (arg1);
    updateInputEndFrameValue();
    updateInputTotalFrameValue();
}

void ImageSequenceConfigPage::on_InputEndFrame_valueChanged(int arg1)
{
    qint32 start=InputStartFrame->value();
    if (arg1<start) InputEndFrame->setValue(start);
    updateInputTotalFrameValue();
}

void ImageSequenceConfigPage::on_InputTotalFrame_valueChanged(int arg1)
{
    Q_UNUSED (arg1);
    updateInputEndFrameValue();
}


void ImageSequenceConfigPage::setRepeatLoop(bool loop)
{
    if (loop != checkLoop->isChecked())
        checkLoop->setChecked(loop);
}

void ImageSequenceConfigPage::setEnableFrameExtractor(bool s)
{
    if (!checkBoxExtractor) return;
    checkBoxExtractor->setChecked(s);
}

bool ImageSequenceConfigPage::getEnableFrameExtractor() const
{
    if (!checkBoxExtractor) return false;
    return checkBoxExtractor->isChecked();
}

void ImageSequenceConfigPage::on_InputPath_textChanged(const QString &text){
    Q_UNUSED(text);
    on_cbDecodeGeometryFromFileName_toggled(cbDecodeGeometryFromFileName->isChecked());
}



void ImageSequenceConfigPage::on_cbDecodeGeometryFromFileName_toggled(bool checked)
{
    if (!checked){
        inputImageH->setValue(0);
        inputImageW->setValue(0);
    }else{
        QSize isize;
        int iColorDepth=0;
        getGeometryFromFilename(InputPath->text(),isize,iColorDepth);
        qDebug()<<"on_cbDecodeGeometryFromFileName_toggled iColorDepth"<<iColorDepth;
        if (!isize.isEmpty()){
            inputImageH->setValue(isize.height());
            inputImageW->setValue(isize.width());
        }
        if (iColorDepth>0){
            int ci=cbColorTypeInput->findText(QString("%1").arg(iColorDepth),Qt::MatchStartsWith);
            if (ci!=-1) cbColorTypeInput->setCurrentIndex(ci);
        }
    }
}

void ImageSequenceConfigPage::getGeometryFromFilename(QString filename, QSize &size, int &depth){
    if (!filename.isEmpty()){
        QFileInfo fileinfo = QFileInfo(filename);
        if (&size){
            QRegularExpression re = QRegularExpression(QString("%1(\\d+)x(\\d+)%1").arg(dataImageSeparator));
            QRegularExpressionMatch match = re.match(fileinfo.completeBaseName());
            if (match.hasMatch()) {
                size.setWidth(match.captured(1).toInt());
                size.setHeight(match.captured(2).toInt());
            }
        }
        if (&depth){
            QRegularExpression re = QRegularExpression(QString("%1(\\d+)bit").arg(dataImageSeparator));
            QRegularExpressionMatch match = re.match(fileinfo.completeBaseName());
            if (match.hasMatch()) {
                depth=match.captured(1).toInt();
            }
        }
    }
}
