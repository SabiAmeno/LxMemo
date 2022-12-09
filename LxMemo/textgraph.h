#ifndef TEXTGRAPH_H
#define TEXTGRAPH_H

#include "graph.h"
#include <QString>
#include <QTimer>
#include <QTextEdit>
#include <QTextObjectInterface>

struct TextStyle
{
	QFont font_{ "Microsoft YaHei UI", 10 };
	QColor color_{ Qt::black };
	QColor bk_color_{ Qt::transparent };
	int border_style_{ 0 };

	friend QDataStream& operator<<(QDataStream& out, const TextStyle& style);
	friend QDataStream& operator>>(QDataStream& out, TextStyle& style);
};

//class TextCursor;
class TextGraph : public Graph
{
public:
	explicit TextGraph(GraphicCanvas* parent, const QString& text = "");
	~TextGraph();

	void SetText(const QString& text);
	TextStyle GetTextStyle() const;
	void SetTextStyle(const TextStyle& style);

	virtual QRect Geometry() override;
	virtual void MouseClick(const QPoint& pos) override;
	virtual void MouseDoubleClick(const QPoint&) override;
	virtual void SerializeTo(QDataStream& stream) override;
	virtual void DeserializeFrom(QDataStream& stream) override;
protected:
	virtual void draw(QPainter& p) override;
	virtual GraphType type() override;
private:
	void updateSize();
private:
	QString text_{};
	//QFont font_{};
	TextStyle text_style_{};

	QTextEdit* edit_{ nullptr };
};

#endif // TEXTGRAPH_H
