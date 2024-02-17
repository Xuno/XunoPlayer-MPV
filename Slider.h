/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2010 Ricardo Villalba <rvm@escomposlinux.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

//TODO: hover support(like youtube and ExMplayer timeline preview)

#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QFrame>
#include <QPainter>
#include <QLabel>
#include <QResizeEvent>

class Slider : public QSlider
{
	Q_OBJECT
public:
    Slider(QWidget *parent = 0);
    ~Slider();
    void addVisualLimits(int min=0, int max=0);
    void setVisualMinLimit(int min);
    void setVisualMaxLimit(int max);
    void clearLimits();
    void updateLimitBar();
    void setVisibleVisualLimit(bool s);

signals:
    void onEnter();
    void onLeave();
    void onHover(int pos, int value);

protected:
    void addLimitBar();
    virtual void enterEvent(QEnterEvent* event);
    virtual void leaveEvent(QEvent *e);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    //virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
//#if CODE_FOR_CLICK == 1
	inline int pick(const QPoint &pt) const;
	int pixelPosToRangeValue(int pos) const;
    void initStyleOption_Qt430(QStyleOptionSlider *option) const;
//#endif
private:
    QFrame *line = 0;
    int visualLimitsMin = 0;
    int visualLimitsMax = 0;
};

#endif //SLIDER_H

