#ifndef GRAPHGOUP_H
#define GRAPHGOUP_H

#include "graph.h"
#include <QVector>

class GraphGroup : public Graph
{
public:
    explicit GraphGroup(GraphicCanvas* canvas);

    void AddGraph(Graph* graph);
    bool DeleteGraph(Graph* graph);
    bool DeleteAll();
    Graph *At(const QPoint& pos);

    void MoveUp(Graph* graph);
    void MoveDown(Graph* graph);
    void MoveTop(Graph* graph);
    void MoveBottom(Graph* graph);

    Graph *SelectGraph();
    virtual void MouseClick(const QPoint& pos) override;
    virtual void MouseDoubleClick(const QPoint& ) override;
    virtual void SerializeTo(QDataStream& stream) override;
    virtual void DeserializeFrom(QDataStream& stream) override;
protected:
    virtual void draw(QPainter& p) override;
    virtual GraphType type() override;
private:
//    void MoveTo();
private:
    QVector<Graph*> graphs_{};
};

#endif // GRAPHGOUP_H
