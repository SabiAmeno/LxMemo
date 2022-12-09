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
	void onShowTextStyle(Graph* graph, bool isleft);
protected:
	virtual void resizeEvent(QResizeEvent* e) override;
private:
	Ui::CanvasClass ui;

	SharedGraph meta_{ nullptr };
	DockWindow* dock_{ nullptr };
	DockWindow* prop_dock_{ nullptr };
	TextStyleWidget* style_widget_{ nullptr };

	MetaSaver* meta_saver_{ nullptr };
};
