#include "textgraph.h"
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QTextEdit>
#include "graphiccanvas.h"

TextGraph::TextGraph(GraphicCanvas *parent, const QString &text)
    : Graph{parent},
      font_{"Microsoft YaHei UI", 10}
{
    edit_ = new QTextEdit(parent);
    edit_->resize(150, 150);
    SetText(text);
    SetSize(QSize(100, 40));
//    cursor_ = new TextCursor(this);
}

void TextGraph::SetText(const QString &text)
{
    text_ = text;
    edit_->setText(text);

//    updateSize();
}

QRect TextGraph::Geometry()
{
    auto rect = QRect(pos_, size_);
//    rect.adjust(-1, -1, 1, 1);

    return rect;
}

void TextGraph::MouseClick(const QPoint &pos)
{
    SetSelected(Geometry().contains(pos));
    if(edit_->isVisible()) {
        text_ = edit_->toPlainText();
        edit_->hide();
    }
//    updateSize();
}

void TextGraph::MouseDoubleClick(const QPoint & pos)
{
//    text_cursor.setPosition(0);
//    edit_->setText(text_);
    edit_->resize(size_);
    edit_->move(pos_);
    edit_->show();
}

void TextGraph::SerializeTo(QDataStream &stream)
{
    Graph::SerializeTo(stream);

    stream << font_;
    stream << text_;
}

void TextGraph::DeserializeFrom(QDataStream &stream)
{
    Graph::DeserializeFrom(stream);

    stream >> font_;
    stream >> text_;

    SetText(text_);
}

void TextGraph::draw(QPainter &p)
{
    Graph::draw(p);

    p.save();

    auto geo = Geometry();
    geo.adjust(3,3,-3,-3);
    p.setFont(font_);

    QTextOption opt;
    opt.setWrapMode(QTextOption::WordWrap);
    p.drawText(geo, text_, opt);
//    p.translate(pos_);
//    doc_->documentLayout()->setPaintDevice(p.device());
//    doc_->setPageSize(size_);
//    auto doc = edit_->document();
//    doc->drawContents(&p);

    p.restore();
}

GraphType TextGraph::type()
{
    return GraphType::kGraphText;
}

void TextGraph::updateSize()
{
    QFontMetrics mtc(font_);
    size_ = mtc.size(Qt::TextExpandTabs, text_);
//    doc_->setTextWidth(size_.width());
}
