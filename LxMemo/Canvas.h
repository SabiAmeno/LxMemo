#pragma once

#include <QWidget>
#include "NoFrameWidget.h"
#include "ui_Canvas.h"

class LxMemo;
class Canvas : public NoFrameWidget
{
	Q_OBJECT

public:
	Canvas(LxMemo* parent);
	~Canvas();

	void SetMeta(SharedGraph meta);
private slots:
	void onCanvasSave();
private:
	Ui::CanvasClass ui;

	SharedGraph meta_{ nullptr };
	LxMemo* lxmemo_{ nullptr };
};
