#include "Canvas.h"
#include "LxMemo.h"
#include <QDateTime>

Canvas::Canvas(LxMemo* parent)
	: NoFrameWidget(parent),
	lxmemo_(parent)
{
	ui.setupUi(this);

	//自定义标题栏行为
	connect(ui.widget, &TitleBar::closeButtonClicked, this, [this] { onCanvasSave(); close(); });
	connect(ui.widget, &TitleBar::minimumButtonClicked, this, [this] {showMinimized(); });
	connect(ui.widget, &TitleBar::positionChanged, this, [this](const QPoint& diff) {move(pos() + diff); });
}

Canvas::~Canvas()
{}

void Canvas::SetMeta(SharedGraph meta)
{
	meta_ = meta;

	ui.canvas->DeserializeFrom(meta_->Data());
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

	lxmemo_->saver_->Push(event);
}