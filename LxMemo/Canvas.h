#pragma once

#include <QWidget>
#include "NoFrameWidget.h"
#include "ui_Canvas.h"
#include "dockwindow.h"
#include "TextStyleWidget.h"

class MetaSaver;
class Canvas : public NoFrameWidget
{
	Q_OBJECT

public:
	Canvas(QWidget* parent = nullptr);
	~Canvas();

	void SetMetaSaver(MetaSaver* ms);
	void SetMeta(SharedGraph meta);
private slots:
	void onCanvasSave();
	void onShowTextStyle(Graph* graph);
protected:
	virtual void resizeEvent(QResizeEvent* e) override;
private:
	Ui::CanvasClass ui;

	SharedGraph meta_{ nullptr };
	DockWindow* dock_{ nullptr };

	MetaSaver* meta_saver_{ nullptr };
};
