#include "textgraph.h"
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QTextEdit>
#include "graphiccanvas.h"

TextGraph::TextGraph(GraphicCanvas* parent, const QString& text)
	: Graph{ parent }
{
	edit_ = new QTextEdit(parent);
	edit_->resize(150, 150);
	edit_->setFont(text_style_.font_);
	edit_->hide();

	SetText(text);
	SetSize(QSize(100, 40));
	//    cursor_ = new TextCursor(this);
}

TextGraph::~TextGraph()
{
	edit_->hide();
	delete edit_;
}

void TextGraph::SetText(const QString& text)
{
	text_ = text;
	edit_->setText(text);

	//    updateSize();
}

TextStyle TextGraph::GetTextStyle() const
{
	return text_style_;
}

void TextGraph::SetTextStyle(const TextStyle& style)
{
	text_style_ = style;

	canvas_->update();
}

QRect TextGraph::Geometry()
{
	auto rect = QRect(pos_, size_);
	//    rect.adjust(-1, -1, 1, 1);

	return rect;
}

void TextGraph::MouseClick(const QPoint& pos)
{
	SetSelected(Geometry().contains(pos));
	if (edit_->isVisible()) {
		text_ = edit_->toPlainText();
		edit_->hide();
	}
	//    updateSize();
}

void TextGraph::MouseDoubleClick(const QPoint& pos)
{
	//    text_cursor.setPosition(0);
	//    edit_->setText(text_);
	edit_->resize(size_ - QSize(1, 1));
	edit_->move(pos_ + QPoint(1, 1));
	edit_->show();
}

void TextGraph::SerializeTo(QDataStream& stream)
{
	Graph::SerializeTo(stream);

	//stream << font_;
	stream << text_style_;
	stream << text_;
}

void TextGraph::DeserializeFrom(QDataStream& stream)
{
	Graph::DeserializeFrom(stream);

	//stream >> font_;
	stream >> text_style_;
	stream >> text_;

	SetText(text_);
}

void TextGraph::draw(QPainter& p)
{
	Graph::draw(p);

	if (!edit_->isVisible()) {
		p.save();

		auto geo = Geometry();
		geo.adjust(3, 3, -3, -3);
		p.setFont(text_style_.font_);

		p.fillRect(geo, text_style_.bk_color_);
		QTextOption opt;
		opt.setWrapMode(QTextOption::WordWrap);
		p.setPen(text_style_.color_);
		p.drawText(geo, text_, opt);
		//    p.translate(pos_);
		//    doc_->documentLayout()->setPaintDevice(p.device());
		//    doc_->setPageSize(size_);
		//    auto doc = edit_->document();
		//    doc->drawContents(&p);

		p.restore();
	}
}

GraphType TextGraph::type()
{
	return GraphType::kGraphText;
}

void TextGraph::updateSize()
{
	QFontMetrics mtc(text_style_.font_);
	size_ = mtc.size(Qt::TextExpandTabs, text_);
	//    doc_->setTextWidth(size_.width());
}

QDataStream& operator<<(QDataStream& out, const TextStyle& style)
{
	out << style.font_;
	out << style.color_;
	out << style.bk_color_;
	out << style.border_style_;

	return out;
}

QDataStream& operator>>(QDataStream& out, TextStyle& style)
{
	out >> style.font_;
	out >> style.color_;
	out >> style.bk_color_;
	out >> style.border_style_;

	return out;
}
