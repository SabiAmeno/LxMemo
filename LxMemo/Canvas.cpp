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
	dock_->SetSize(QSize(200, 30));
	dock_->SetStickArea(StickArea::kBottom);

	auto txt_button = new QToolButton();
	auto pix_button = new QToolButton();
	auto graph_button = new QToolButton();
	txt_button->setFixedSize(24, 24);
	pix_button->setFixedSize(24, 24);
	graph_button->setFixedSize(24, 24);
	txt_button->setText("TXT");
	pix_button->setText("PIX");
	graph_button->setText("GRP");

	dock_->AddWidget(txt_button);
	dock_->AddWidget(pix_button);
	dock_->AddWidget(graph_button);

	ui.property_widget->hide();
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

void Canvas::onShowTextStyle(Graph* graph)
{
	if(graph)
		ui.property_widget->SetGraph(graph);
	ui.property_widget->setVisible(nullptr != graph);
}

void Canvas::resizeEvent(QResizeEvent* e)
{
	dock_->Update();
	//style_dock_->Update();

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