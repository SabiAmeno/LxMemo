#include "GraphNote.h"

GraphNote::GraphNote(uint32_t id)
	: Meta(id)
{
}

void GraphNote::paint(QPainter& painter, const QRect& rect) const
{
	QPixmap pix(":/LxMemo/icons/memo_main.png");
	painter.drawPixmap(rect, pix.scaled(rect.size(), Qt::KeepAspectRatio));

	painter.restore();
}

size_t GraphNote::type()
{
	return typeid(GraphNote).hash_code();
}

void GraphNote::SetData(const QByteArray& data)
{
	data_ = data;
}

QByteArray GraphNote::Data()
{
	return data_;
}