#CONFIG -= app_bundle
#CONFIG += console
#CONFIG += windeployqt

TEMPLATE = app

QT += widgets
QT += webenginewidgets
QT += sql


PROJECTROOT = $$PWD

win32: {
 message("XunoPlayer-MPV WINDOWS")
 devtools=D:/develop-tools/
 QT += winextras
 QT_CONFIG -= no-pkg-config
 CONFIG += link_pkgconfig
 QT_CONFIG -= no-pkg-config
} else:unix:!macx:{
 message("XunoPlayer-MPV UNIX")
 devtools=/home/lex/develop-tools
}


TARGET = XunoPlayer-MPV
VER_MAJ = 0
VER_MIN = 0
VER_PAT = 10
VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}

GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
GIT_VERSION += \{$$system(git rev-parse --abbrev-ref HEAD)\}
VERGITSTR = '\\"$${GIT_VERSION}\\"'

DEFINES += VER_MAJ_STRING=$${VER_MAJ}
DEFINES += VER_MIN_STRING=$${VER_MIN}
DEFINES += VER_PAT_STRING=$${VER_PAT}
DEFINES += VERSION_STRING=\\\"$${VER_MAJ}.$${VER_MIN}.$${VER_PAT}\\\"
DEFINES += VERGIT=\"$${VERGITSTR}\"

RC_ICONS = $${PROJECTROOT}/XunoPlayer-MPV_128x128.ico
QMAKE_TARGET_COMPANY = "Aaex Corp. http://www.xuno.com. https://github.com/Xuno/XunoPlayer-MPV"
QMAKE_TARGET_DESCRIPTION = "XunoPlayer-MPV. Aaex Corp. http://www.xuno.com."
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2012-2018 Aaex Corp."
QMAKE_TARGET_PRODUCT = "XunoPlayer-MPV $$1 VERSION GIT_VERSION"
export(RC_ICONS)
export(QMAKE_TARGET_COMPANY)
export(QMAKE_TARGET_DESCRIPTION)
export(QMAKE_TARGET_COPYRIGHT)
export(QMAKE_TARGET_PRODUCT)





HEADERS = \
    XunoPlayerMpv.h \
    XunoBrowser.h \
    mpvwidget.h \
    Slider.h \
    ClickableMenu.h \
    common/Config.h \
    common/common.h \
    common/common_export.h \
    common/qoptions.h \
    common/ScreenSaver.h \
    EventFilter.h \
    config/ConfigDialog.h \
    config/ConfigPageBase.h \
    config/configwebmemu.h \
    config/WebConfigPage.h \
    config/ImageSequenceConfigPage.h \
    config/VideoEQConfigPage.h \
    config/MiscPage.h \
    playlist/PlayList.h \
    playlist/PlayListDelegate.h \
    playlist/PlayListItem.h \
    playlist/PlayListModel.h \
    StatisticsView.h \
    Statistics.h \
    DarkStyle.h

#    mpv/client.h \
#    mpv/opengl_cb.h \
#    mpv/qthelper.hpp \
#    mpv/stream_cb.h \



#    playlist/PlayList.h \
#    playlist/PlayListDelegate.h \
#    playlist/PlayListItem.h \
#    playlist/PlayListModel.h \


SOURCES = main.cpp \
    XunoPlayerMpv.cpp \
    ClickableMenu.cpp \
    Slider.cpp \
    mpvwidget.cpp \
    XunoBrowser.cpp \
    common/common.cpp \
    common/Config.cpp \
    common/qoptions.cpp \
    common/ScreenSaver.cpp \
    EventFilter.cpp \
    config/ConfigDialog.cpp \
    config/configwebmemu.cpp \
    config/WebConfigPage.cpp \
    config/ConfigPageBase.cpp \
    config/ImageSequenceConfigPage.cpp \
    config/MiscPage.cpp \
    playlist/PlayList.cpp \
    playlist/PlayListDelegate.cpp \
    playlist/PlayListItem.cpp \
    playlist/PlayListModel.cpp \
    config/VideoEQConfigPage.cpp \
    StatisticsView.cpp \
    Statistics.cpp \
    DarkStyle.cpp

#    playlist/PlayList.cpp \
#    playlist/PlayListDelegate.cpp \
#    playlist/PlayListItem.cpp \
#    playlist/PlayListModel.cpp \


RESOURCES += \
    theme.qrc \
    darkstyle.qrc

FORMS += \
    config/configwebmemu.ui

message("XunoPlayer-MPV DEFINES: "$$DEFINES)

DISTFILES += \
    darkstyle/icon_branch_closed.png \
    darkstyle/icon_branch_end.png \
    darkstyle/icon_branch_more.png \
    darkstyle/icon_branch_open.png \
    darkstyle/icon_checkbox_checked.png \
    darkstyle/icon_checkbox_checked_disabled.png \
    darkstyle/icon_checkbox_checked_pressed.png \
    darkstyle/icon_checkbox_indeterminate.png \
    darkstyle/icon_checkbox_indeterminate_disabled.png \
    darkstyle/icon_checkbox_indeterminate_pressed.png \
    darkstyle/icon_checkbox_unchecked.png \
    darkstyle/icon_checkbox_unchecked_disabled.png \
    darkstyle/icon_checkbox_unchecked_pressed.png \
    darkstyle/icon_close.png \
    darkstyle/icon_radiobutton_checked.png \
    darkstyle/icon_radiobutton_checked_disabled.png \
    darkstyle/icon_radiobutton_checked_pressed.png \
    darkstyle/icon_radiobutton_unchecked.png \
    darkstyle/icon_radiobutton_unchecked_disabled.png \
    darkstyle/icon_radiobutton_unchecked_pressed.png \
    darkstyle/icon_undock.png \
    darkstyle/icon_vline.png \
    images/icon_window_close.png \
    images/icon_window_maximize.png \
    images/icon_window_minimize.png \
    images/icon_window_restore.png \
    darkstyle/darkstyle.qss

#MPV

win32:{
#MPVDIR=$${devtools}/mpv/mpvlib/mpv-dev-20181002
MPVDIR=$${devtools}/mpv/byXunoBuild/build-shared-libmpv-2019-05-01
LIBS += -L$${MPVDIR}/ -llibmpv
#HEADERS += \
#    mpv/client.h \
#    mpv/opengl_cb.h \
#    mpv/qthelper.hpp \
#    mpv/stream_cb.h

INCLUDEPATH += $${MPVDIR}/include
DEPENDPATH += $${MPVDIR}/include

}
else:unix:!macx:{

MPVDIR=$${devtools}/mpv-player/mpv-build/build-XunoMpv-20190206
#MPVDIR=$${devtools}/mpv-player/mpv-build
FFMPEGDIR=$${devtools}/ffmpeg/ffmpeg_sources/ffmpeg-build

#QT_CONFIG -= no-pkg-config
#INCLUDEPATH +=$${devtools}/mpv-player/git/mpv-build/build_libs/include
#INCLUDEPATH +=$${devtools}/mpv-player/git/mpv-build/build_libs/include/libavformat
#INCLUDEPATH +=$${devtools}/mpv-player/git/mpv-build/build_libs/include/libavcodec
#INCLUDEPATH +=$${devtools}/mpv-player/git/mpv-build/build_libs/include/libavutil
#INCLUDEPATH +=$${devtools}/mpv-player/git/mpv-build/build_libs/include/libswresample

#INCLUDEPATH +=$${devtools}/ffmpeg/ffmpeg_sources/ffmpeg-build/include
#INCLUDEPATH +=$${devtools}/ffmpeg/ffmpeg_sources/ffmpeg-build/include/libavformat
#INCLUDEPATH +=$${devtools}/ffmpeg/ffmpeg_sources/ffmpeg-build/include/libavcodec
#LIBS += -L$${devtools}/mpv-player/mpv-build/build-XunoMpv-20180404/lib

LIBS += -L$${FFMPEGDIR}/lib
LIBS += -L$${MPVDIR}/lib


#PKG_CONFIG_PATH += $${PROJECTROOT}/pkgconfig:$PKG_CONFIG_PATH


#PKG_CONFIG_PATH += $${devtools}/mpv-player/git/mpv-build/build_libs/lib/pkgconfig:$PKG_CONFIG_PATH

CONFIG += link_pkgconfig

PKG_CONFIG_PATH += $${FFMPEGDIR}/lib/pkgconfig

PKGCONFIG += libavformat \
             libavutil \
             libswresample \
             libavcodec


#PKGCONFIG += $${FFMPEGDIR}/lib/pkgconfig/libavformat.pc
#PKGCONFIG += $${FFMPEGDIR}/lib/pkgconfig/libavutil.pc
#PKGCONFIG += $${FFMPEGDIR}/lib/pkgconfig/libswresample.pc
#PKGCONFIG += $${FFMPEGDIR}/lib/pkgconfig/libavcodec.pc

#PKGCONFIG += $${devtools}/mpv-player/mpv-build/build-XunoMpv-20190206/lib/pkgconfig/mpv.pc
PKGCONFIG += $${MPVDIR}/lib/pkgconfig/mpv.pc
#PKGCONFIG += $${PROJECTROOT}/pkgconfig/mpv.pc

LD_LIBRARY_PATH += $${FFMPEGDIR}/lib
LD_LIBRARY_PATH += $${MPVDIR}/lib

}

