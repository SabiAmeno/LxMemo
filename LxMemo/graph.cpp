#include "graph.h"
#include "graphiccanvas.h"

Graph::Graph(GraphicCanvas *parent)
    : QObject(parent), canvas_{parent}
{

}

void Graph::Draw(QPainter &p)
{
    draw(p);
}

GraphType Graph::Type()
{
    return type();
}

void Graph::SetSize(const QSize &size)
{
    size_ = size;
}

void Graph::SetCoord(const QPoint &pos)
{
    pos_ = pos;
}

void Graph::SetColor(const QColor& color)
{
    color_ = color;
}

void Graph::SetSelected(bool sel)
{
    selected_ = sel;
}

bool Graph::Selected()
{
    return selected_;
}

QPoint Graph::Coord()
{
    return pos_;
}

QSize Graph::Size()
{
    return size_;
}

QRect Graph::Geometry()
{
    return QRect(pos_, size_);
}

void Graph::SerializeTo(QDataStream &stream)
{
    stream << type();
    stream << pos_;
    stream << size_;
    stream << color_;
}

void Graph::DeserializeFrom(QDataStream &stream)
{
    stream >> pos_;
    stream >> size_;
    stream >> color_;
}

void Graph::draw(QPainter &p)
{
    p.save();
    if(selected_) {
        auto geo = Geometry();
        p.fillRect(geo, QColor("#efefef"));
        p.drawRect(geo);

        auto top_left_rect = QRect(geo.topLeft() - QPoint(3, 3), QSize(6, 6));
        auto top_right_rect = QRect(geo.topRight() - QPoint(3, 3), QSize(6,6));
        auto bottom_right_rect = QRect(geo.bottomRight() - QPoint(3, 3), QSize(6,6));
        auto bottom_left_rect = QRect(geo.bottomLeft() - QPoint(3,3), QSize(6,6));

        p.drawRect(top_left_rect);
        p.drawRect(top_right_rect);
        p.drawRect(bottom_left_rect);
        p.drawRect(bottom_right_rect);
    }
    p.restore();
}
