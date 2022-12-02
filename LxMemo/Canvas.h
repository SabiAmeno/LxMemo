#pragma once

#include <QWidget>
#include "NoFrameWidget.h"
#include "ui_Canvas.h"

class Canvas : public NoFrameWidget
{
    Q_OBJECT

public:
    Canvas(QWidget *parent = nullptr);
    ~Canvas();

private:
    Ui::CanvasClass ui;
};
