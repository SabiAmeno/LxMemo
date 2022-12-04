#pragma once
#include "Meta.h"
class GraphNote : public Meta
{
public:
	GraphNote() = default;
	GraphNote(uint32_t id);

	virtual void paint(QPainter& painter, const QRect& rect) const;
	virtual size_t type();

	void SetData(const QByteArray& data);
	QByteArray Data();
private:
	QByteArray data_{};
};

using SharedGraph = QSharedPointer<GraphNote>;

Q_DECLARE_METATYPE(SharedGraph);