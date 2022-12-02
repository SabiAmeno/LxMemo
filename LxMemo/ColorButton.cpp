#include "ColorButton.h"
#include <QPainter>
#include <QAction>
#include <QMenu>
#include <QWidgetAction>

ColorButton::ColorButton(QWidget* parent)
    : QToolButton(parent)
{
    QMenu* menu = new QMenu(this);
    /*QWidgetAction* wa = new QWidgetAction(this);

    selector = new ColorSelector(this);
    wa->setDefaultWidget(selector);
    menu->addAction(wa);*/
    setMenu(menu);

    //connect(selector, &ColorSelector::colorChanged, this, &ColorButton::onColorChanged);
}

ColorButton::~ColorButton()
{}

void ColorButton::SetColor(const QColor & color)
{
    _color = color;
    //selector->SetColor(color);

    update();
}

void ColorButton::SetColorFace(FaceColor* fc)
{
    auto me = menu();
    QWidgetAction* wa = new QWidgetAction(this);
    wa->setDefaultWidget(fc);
    me->addAction(wa);

    connect(fc, &FaceColor::colorChanged, this, &ColorButton::onColorChanged);
}

void ColorButton::onColorChanged(const QColor& color)
{
    _color = color;
    
    update();

    emit colorChnaged(color);
}


void ColorButton::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    QPainter p(this);
    QPixmap pix(_icon);

    pix = pix.scaled(QSize(16, 16));

    p.drawPixmap(4, 0, pix);
    p.fillRect(QRect(4, pix.height() + 2, 16, 4), _color);
}