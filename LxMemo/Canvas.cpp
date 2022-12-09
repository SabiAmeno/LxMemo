#include "Canvas.h"
#include "LxMemo.h"
#include <QDateTime>
#include "MetaSaver.h"

Canvas::Canvas(QWidget* parent)
	: NoFrameWidget(parent)
{
	ui.setupUi(this);

	dock_ = new DockWindow(this);
	dock_->SetAutoShrink(false);
	dock_->SetSize(QSize(36, 300));
	dock_->SetStickArea(StickArea::kLeft);
	dock_->SetFixedStick(true);
	//dock_->SetAutoHeight(true);

	auto txt_button = new QToolButton();
	auto pix_button = new QToolButton();
	auto graph_button = new QToolButton();
	auto xx1 = new QToolButton();
	auto xx2 = new QToolButton();

	txt_button->setFixedSize(24, 24);
	pix_button->setFixedSize(24, 24);
	graph_button->setFixedSize(24, 24);
	txt_button->setText("TXT");
	pix_button->setText("PIX");
	graph_button->setText("GRP");

	xx1->setFixedSize(24, 24);
	xx2->setFixedSize(24, 24);
	xx1->setText("X1");
	xx2->setText("X2");

	dock_->AddWidget(txt_button);
	dock_->AddWidget(pix_button);
	dock_->AddWidget(graph_button);
	dock_->AddWidget(xx1);
	dock_->AddWidget(xx2);

	auto pal = dock_->palette();
	pal.setColor(QPalette::Window, QColor(85, 170, 127));
	dock_->setPalette(pal);

	prop_dock_ = new DockWindow(this);
	prop_dock_->SetAutoShrink(false);
	prop_dock_->SetSize(QSize(220, height() - 40));
	prop_dock_->SetStickArea(StickArea::kRight);
	prop_dock_->SetFixedStick(true);
	prop_dock_->SetAutoHeight(true);

	style_widget_ = new TextStyleWidget();
	prop_dock_->AddWidget(style_widget_);

	pal = prop_dock_->palette();
	pal.setColor(QPalette::Window, Qt::transparent);
	prop_dock_->setPalette(pal);

	prop_dock_->hide();

	//ui.property_widget->hide();
	connect(txt_button, &QToolButton::clicked, ui.canvas, &GraphicCanvas::onWordAdd);
	connect(pix_button, &QToolButton::clicked, ui.canvas, &GraphicCanvas::onPictureAdd);
	connect(graph_button, &QToolButton::clicked, ui.canvas, &GraphicCanvas::onGraphAdd);

	connect(ui.canvas, &GraphicCanvas::GraphClicked, this, &Canvas::onShowTextStyle);
	//自定义标题栏行为
	connect(ui.widget, &TitleBar::closeButtonClicked, this, [this] { onCanvasSave(); close(); });
	connect(ui.widget, &TitleBar::minimumButtonClicked, this, [this] {showMinimized(); });
	connect(ui.widget, &TitleBar::maximumButtonClicked, this, [this] {if (!isMaximized()) showMaximized(); else showNormal(); });
	connect(ui.widget, &TitleBar::positionChanged, this, [this](const QPoint& diff) {move(pos() + diff); });
}

Canvas::~Canvas()
{}

void Canvas::SetMetaSaver(MetaSaver* ms)
{
	meta_saver_ = ms;
}

void Canvas::SetMeta(SharedGraph meta)
{
	meta_ = meta;

	ui.canvas->DeserializeFrom(meta_->Data());
}

void Canvas::onShowTextStyle(Graph* graph, bool isleft)
{
	if(graph)
		style_widget_->SetGraph(graph);
	//bool vi = prop_dock_->isVisible();
	//vi = graph && (!isleft || (isleft && vi));
	prop_dock_->setVisible(isleft);
}

void Canvas::resizeEvent(QResizeEvent* e)
{
	dock_->SetViewport(QRect(ui.canvas->pos(), ui.canvas->size()));
	dock_->Update();
	
	//auto pos = 
	prop_dock_->SetViewport(QRect(ui.canvas->pos(), ui.canvas->size()));
	prop_dock_->Update();

	QWidget::resizeEvent(e);
}

void Canvas::onCanvasSave()
{
	auto bytes = ui.canvas->SerializeTo();

	auto time = QDateTime::currentDateTime().toString();

	meta_->SetData(bytes);
	meta_->SetTime(time);

	Event* event = new Event();
	event->meta = meta_;
	event->contents = bytes.toBase64();
	event->time = time;

	meta_saver_->Push(event);
}