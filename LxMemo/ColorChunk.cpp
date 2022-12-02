#include "ColorChunk.h"
#include <QPainter>
#include <QMouseEvent>

ColorChunk::ColorChunk(QWidget* parent)
    : QWidget(parent),
    _colorList{ 0x549BCE, 0XC6B7A4, 0X70C5CA, 0X8CA231, 0XA3D9F2, 0XD3B5FF, 0XE3CCC4 }
{
    setFixedHeight(40);

    //setWindowFlags(Qt::Dialog);
}

ColorChunk::~ColorChunk()
{}

void ColorChunk::AddColor(const QColor& color)
{
    _colorList.append(color);
}

void ColorChunk::paintEvent(QPaintEvent* event)
{
    QPainter p(this);

    int ind = 0;
    p.fillRect(rect(), Qt::white);
    int w = width();
    w = w / _colorList.size();

    for (auto it : _colorList) {
        //p.drawRect(ind * 40, 0, 40, 40);
        p.save();
        p.fillRect(ind * w, 0, w, 40, it);
        p.restore();
        ind++;
    }
}

void ColorChunk::mousePressEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    int w = width();
    w = w / _colorList.size();

    int ind = pos.x() / w;

    emit ColorSelected(_colorList[ind]);

    close();
}