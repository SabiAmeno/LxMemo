#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QPainter>

enum GraphType
{
    kGraphText,
    kGraphPicture,
    kGraphGroup,
    kGraphNone,
};

class GraphicCanvas;
class Graph : public QObject
{
    Q_OBJECT
public:
    explicit Graph(GraphicCanvas *parent = nullptr);

    void Draw(QPainter& p);
    GraphType Type();

    virtual void SetSize(const QSize& size);
    virtual void SetCoord(const QPoint& pos);
    void SetSelected(bool sel);
    bool Selected();

    QPoint Coord();
    QSize Size();

    template<typename Derived>
    Derived* converTo()
    {
        return dynamic_cast<Derived*>(this);
    }

    virtual QRect Geometry();
    virtual void MouseClick(const QPoint&) {}
    virtual void MouseDoubleClick(const QPoint& ) {}
    virtual void SerializeTo(QDataStream& stream);
    virtual void DeserializeFrom(QDataStream& stream);
protected:
    virtual void draw(QPainter& p);
    virtual GraphType type() = 0;
signals:
protected:
    QSize size_{};
    QPoint pos_{};
    bool selected_{false};

    GraphicCanvas* canvas_{nullptr};
};

#endif // GRAPH_H
