#ifndef TEXTGRAPH_H
#define TEXTGRAPH_H

#include "graph.h"
#include <QString>
#include <QTimer>
#include <QTextEdit>
#include <QTextObjectInterface>

//class TextCursor;
class TextGraph : public Graph
{
public:
    explicit TextGraph(GraphicCanvas* parent, const QString& text = "");

    void SetText(const QString& text);

    virtual QRect Geometry() override;
    virtual void MouseClick(const QPoint& pos) override;
    virtual void MouseDoubleClick(const QPoint& ) override;
    virtual void SerializeTo(QDataStream& stream) override;
    virtual void DeserializeFrom(QDataStream& stream) override;
protected:
    virtual void draw(QPainter& p) override;
    virtual GraphType type() override;
private:
    void updateSize();
private:
    QString text_{};
    QFont font_{};

    QTextEdit* edit_{nullptr};
//    TextCursor* cursor_{nullptr};
};


#endif // TEXTGRAPH_H
