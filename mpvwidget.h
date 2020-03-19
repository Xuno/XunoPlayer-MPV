
#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QTime>
#include <QtWidgets/QOpenGLWidget>
//#include <mpv/client.h>
//#include <mpv/opengl_cb.h>

#include "mpv/render_gl.h"
#include "common/qthelper.hpp"

class MpvWidget Q_DECL_FINAL: public QOpenGLWidget
{
    Q_OBJECT
public:
    MpvWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~MpvWidget();
    void command(const QVariant& params);
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    QSize sizeHint() const { return QSize(960, 540);}
    QVariant command_result(const QVariant &params);
    void setOption(const QString &name, const QString parameter);
    qreal current_fps();
    bool getOsdmsg_used() const;
    void setOsdmsg_used(bool value);

Q_SIGNALS:
    void durationChanged(int value);
    void positionChanged(int value);
    void mpv_on_START_FILE();
    void mpv_on_FILE_LOADED();
    void mpv_on_PAUSE();
    void mpv_on_END_FILE();
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
private Q_SLOTS:
    void swapped();
    void on_mpv_events();
    void maybeUpdate();
private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);
    QTime frameTime;
    qreal fps;
    long frameCount;
    bool osdmsg_used=false;

    //mpv::qt::Handle mpv;
    mpv_handle  *mpv;
    //mpv_opengl_cb_context *mpv_gl;
    mpv_render_context *mpv_gl;
    QWidget *mpv_container;
};



#endif // PLAYERWINDOW_H
