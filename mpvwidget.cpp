#include "mpvwidget.h"
#include <QDir>
#include <QDebug>
#include <stdexcept>
#include <QtGui/QOpenGLContext>
#include <QtCore/QMetaObject>

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget*)ctx, "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name) {
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return NULL;
    return (void *)glctx->getProcAddress(QByteArray(name));
}

MpvWidget::MpvWidget(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    mpv = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!mpv)
        throw std::runtime_error("could not create mpv context");


    mpv_set_option_string(mpv, "terminal", "yes");
    //mpv_set_option_string(mpv,"msg-time", "");
    //mpv_set_option_string(mpv, "msg-level", "all=v");

    //XUNO
    mpv_set_option_string(mpv, "opengl-pbo", "yes");
//    QString curdir=QDir::currentPath();
//    QString shaderdir=QDir::currentPath().append("/shaders/gather/ravu-r4-yuv.hook");
//    qDebug()<<"shaderdir"<<shaderdir;
    //mpv_set_option_string(mpv, "glsl-shaders", shaderdir.toLatin1().data());

    mpv_set_option_string(mpv, "glsl-shaders", "./shaders/gather/ravu-r4-yuv.hook");

    //mpv_set_option_string(mpv, "input-conf", "lex.conf");
    mpv_set_option_string(mpv, "input-conf", "input.conf");


//    mpv_set_option_string(mpv, "load-scripts", "yes");
//    mpv_set_option_string(mpv, "script", "ytdl_hook.lua");

    //mpv_set_option_string(mpv, "script", "osc.lua");
    //mpv_load_config_file(mpv,"input.conf");


//    mpv_set_option_string(mpv, "", "");
//    mpv_set_option_string(mpv, "", "");

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Make use of the MPV_SUB_API_OPENGL_CB API.

    //mpv::qt::set_option_variant(mpv, "player-operation-mode", "pseudo-gui");

   // mpv::qt::set_option_variant(mpv, "osc", "");
    //mpv::qt::set_option_variant(mpv, "script", "osc.lua");
  //  mpv::qt::set_option_variant(mpv, "script", "ytdl_hook.lua");


    mpv_load_config_file(mpv,"./mpv.conf");

    mpv_set_option_string(mpv,"no-term-osd-bar", "");

    mpv::qt::set_option_variant(mpv, "vo", "opengl-cb");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");
    mpv::qt::set_option_variant(mpv, "scale", "ewa_lanczossharp");
    mpv::qt::set_option_variant(mpv, "dscale", "sinc");
    mpv::qt::set_option_variant(mpv, "cscale", "ewa_lanczossharp");
    mpv::qt::set_option_variant(mpv, "deband-iterations", 1);
    mpv::qt::set_option_variant(mpv, "deband-threshold", 70);
    mpv::qt::set_option_variant(mpv, "deband-range", 16);
    mpv::qt::set_option_variant(mpv, "deband-grain", 5);
    mpv::qt::set_option_variant(mpv, "interpolation", "yes");
    mpv::qt::set_option_variant(mpv, "video-sync", "display-resample");
    mpv::qt::set_option_variant(mpv, "tscale", "sinc");
    mpv::qt::set_option_variant(mpv, "cache", "yes");

    mpv::qt::set_option_variant(mpv, "cache-default", 500000);
    mpv::qt::set_option_variant(mpv, "cache-backbuffer", 250000);
    mpv::qt::set_option_variant(mpv, "cache-secs", 100);

    //mpv::qt::set_option_variant(mpv, "osc", "yes");

    // mpv::qt::set_option_variant(mpv, "input-conf", "lex.conf");
    //mpv_set_option_string(mpv, "input-conf", "lex.conf");


    mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);
    if (!mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");
    mpv_opengl_cb_set_update_callback(mpv_gl, MpvWidget::on_update, (void *)this);
    connect(this, SIGNAL(frameSwapped()), SLOT(swapped()));

    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_set_wakeup_callback(mpv, wakeup, this);

    //mpv::qt::command_variant(mpv, QVariantList()<<"load-script"<<"ytdl_hook.lua");
}

MpvWidget::~MpvWidget()
{
    makeCurrent();
    if (mpv_gl)
        mpv_opengl_cb_set_update_callback(mpv_gl, NULL, NULL);
    // Until this call is done, we need to make sure the player remains
    // alive. This is done implicitly with the mpv::qt::Handle instance
    // in this class.
    mpv_opengl_cb_uninit_gl(mpv_gl);
}

void MpvWidget::command(const QVariant& params)
{
    mpv::qt::command_variant(mpv, params);
}

QVariant MpvWidget::command_result(const QVariant& params)
{
    return mpv::qt::command(mpv, params);
}

void MpvWidget::setOption(const QString &name, const QString parameter)
{
    mpv_set_option_string(mpv, name.toLatin1().data(), parameter.toLatin1().data());
}

qreal MpvWidget::current_fps()
{
    return fps;
}

void MpvWidget::setProperty(const QString& name, const QVariant& value)
{
    mpv::qt::set_property_variant(mpv, name, value);
}

QVariant MpvWidget::getProperty(const QString &name) const
{
    return mpv::qt::get_property_variant(mpv, name);
}

void MpvWidget::initializeGL()
{
    int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");
    frameTime.start();
}

void MpvWidget::paintGL()
{
    mpv_opengl_cb_draw(mpv_gl, defaultFramebufferObject(), width(), -height());
    ++frameCount;
    if (frameTime.elapsed() >= 1000)
    {
        fps = frameCount / ((double)frameTime.elapsed()/1000.0);
        frameCount=0;
        frameTime.restart();
    }
}

void MpvWidget::swapped()
{
    mpv_opengl_cb_report_flip(mpv_gl, 0);
}

void MpvWidget::on_mpv_events()
{
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        handle_mpv_event(event);
    }
}

void MpvWidget::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        if (strcmp(prop->name, "time-pos") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data;
                Q_EMIT positionChanged(time);
            }
        } else if (strcmp(prop->name, "duration") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data;
                Q_EMIT durationChanged(time);
            }
        }
        break;
    }
    case MPV_EVENT_START_FILE:{
        Q_EMIT mpv_on_START_FILE();
        break;
    }
    case MPV_EVENT_FILE_LOADED:{
        Q_EMIT mpv_on_FILE_LOADED();
        break;
    }
    case MPV_EVENT_PAUSE:{
        Q_EMIT mpv_on_PAUSE();
        break;
    }
    case MPV_EVENT_END_FILE:{
        Q_EMIT mpv_on_END_FILE();
        break;
    }


    default: ;
        // Ignore uninteresting or unknown events.
    }
}

// Make Qt invoke mpv_opengl_cb_draw() to draw a new/updated video frame.
void MpvWidget::maybeUpdate()
{
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's opengl-cb API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.
    if (window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        swapped();
        doneCurrent();
    } else {
        update();
    }
}

void MpvWidget::on_update(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget*)ctx, "maybeUpdate");
}

bool MpvWidget::getOsdmsg_used() const
{
    return osdmsg_used;
}

void MpvWidget::setOsdmsg_used(bool value)
{
    osdmsg_used = value;
}

