#include "Canvas.h"

Canvas::Canvas(QWidget *parent)
    : NoFrameWidget(parent)
{
    ui.setupUi(this);

    //自定义标题栏行为
    connect(ui.widget, &TitleBar::closeButtonClicked, this, [this] { close(); });
    connect(ui.widget, &TitleBar::minimumButtonClicked, this, [this] {showMinimized(); });
    connect(ui.widget, &TitleBar::positionChanged, this, [this](const QPoint& diff) {move(pos() + diff); });
}

Canvas::~Canvas()
{}
