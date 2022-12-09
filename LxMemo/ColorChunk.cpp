#include "ColorChunk.h"
#include <QPainter>
#include <QMouseEvent>

ColorChunk::ColorChunk(QWidget* parent)
    : QWidget(parent),
    _colorList{ 0x549BCE, 0XC6B7A4, 0X70C5CA, 0X8CA231, 0XA3D9F2, 0XD3B5FF, 0XE3CCC4, 0XC7C7C7, 0X55AAFF }
{
    //setFixedHeight(20);
    setFixedSize(120, 120);
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

    //int ind = 0;
    //p.fillRect(rect(), Qt::white);
    //int w = width();
    //w = w / _colorList.size();

    //for (auto it : _colorList) {
    //    //p.drawRect(ind * 40, 0, 40, 40);
    //    p.save();
    //    p.fillRect(ind * w, 0, w, 40, it);
    //    p.restore();
    //    ind++;
    //}

    int size = _colorList.size();
    int col = sqrt(size);
    int row = col;
    if (row * col < size)
        row++;

    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            if (r * col + c >= size)
                return;
            int x = 40 * c;
            int y = 40 * r;
            p.fillRect(x, y, 40, 40, _colorList.at(r * col + c));
        }
    }
}

void ColorChunk::mousePressEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    //int w = width();
    //w = w / _colorList.size();

    //int ind = pos.x() / w;

    int x = pos.x() / 40;
    int y = pos.y() / 40;

    int size = _colorList.size();
    int col = sqrt(size);

    emit ColorSelected(_colorList[y * col + x]);

    close();
}