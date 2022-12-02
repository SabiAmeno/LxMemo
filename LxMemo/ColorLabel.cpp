#include "ColorLabel.h"
#include <QPainter>

ColorLabel::ColorLabel(QWidget *parent)
    : QWidget(parent)
{}

ColorLabel::~ColorLabel()
{}

void ColorLabel::SetColor(const QColor & color)
{
    _color = color;
    update();
}

void ColorLabel::paintEvent(QPaintEvent* event)
{
    QPainter p(this);

    int w = width(), h = height();
    QPoint c = rect().center();

    p.fillRect(rect(), Qt::white);
    p.fillRect(c.x() - w / 2 + 4, c.y() - h / 2 + 4, w - 5, h - 5, _color);
}
