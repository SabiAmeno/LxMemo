#ifndef GRAPHICGRAPH_H
#define GRAPHICGRAPH_H

#include "graph.h"
#include <QPixmap>

class GraphicGraph : public Graph
{
    Q_OBJECT
public:
    GraphicGraph(GraphicCanvas *parent = nullptr);
    ~GraphicGraph();

    QString GraphicUrl();

    void SetPicture(const QString& file);
    void SetPicture(const QPixmap& pixmap);
    virtual void SetSize(const QSize& size) override;
//    virtual QRect Geometry() override;
    virtual void MouseClick(const QPoint& pos) override;
    virtual void MouseDoubleClick(const QPoint& ) override;
    virtual void SerializeTo(QDataStream& stream) override;
    virtual void DeserializeFrom(QDataStream& stream) override;
protected:
    virtual void draw(QPainter& p) override;
    virtual GraphType type() override;
private slots:
    void onPictureSelected(const QString& file);
private:
    QPixmap pixmap_{};
    QPixmap origin_pixmap_{};

    QString file_{};
    //QFileDialog* file_dialog_{nullptr};
};

#endif // GRAPHICGRAPH_H
