#include "graphicgraph.h"
#include "graphiccanvas.h"
#include <QStandardPaths>

GraphicGraph::GraphicGraph(GraphicCanvas *parent)
    : Graph(parent)
{
    origin_pixmap_ = QPixmap(":/LxMemo/icons/picture.png");
    SetSize(QSize(60, 60));

    file_dialog_ = new QFileDialog();
    file_dialog_->setViewMode(QFileDialog::Detail);
    file_dialog_->setFilter(QDir::Files);
    file_dialog_->setNameFilter("JPEG(*.jpg,*.jpeg);PNG(*.png)");

    connect(file_dialog_, &QFileDialog::fileSelected, this, &GraphicGraph::onPictureSelected);
}

GraphicGraph::~GraphicGraph()
{
    file_dialog_->close();
    delete file_dialog_;
}

QString GraphicGraph::GraphicUrl()
{
    return file_;
}

void GraphicGraph::SetPicture(const QString &file)
{
    onPictureSelected(file);
}

void GraphicGraph::SetPicture(const QPixmap &pixmap)
{
    origin_pixmap_ = pixmap;
    SetSize(size_);
}

void GraphicGraph::SetSize(const QSize &size)
{
    Graph::SetSize(size);

    pixmap_ = origin_pixmap_.scaled(size_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void GraphicGraph::MouseClick(const QPoint &pos)
{
    SetSelected(Geometry().contains(pos));
}

void GraphicGraph::MouseDoubleClick(const QPoint &)
{
    file_dialog_->show();
}

void GraphicGraph::SerializeTo(QDataStream &stream)
{
    Graph::SerializeTo(stream);

    stream << file_;
}

void GraphicGraph::DeserializeFrom(QDataStream &stream)
{
    Graph::DeserializeFrom(stream);

    stream >> file_;
    SetPicture(file_);
}

void GraphicGraph::draw(QPainter &p)
{
    Graph::draw(p);

    p.save();

    auto geo = Geometry();
    if(Selected())
        p.drawRect(geo);

    geo.adjust(3, 3, -3, -3);
    p.drawPixmap(geo, pixmap_);

    p.restore();
}

GraphType GraphicGraph::type()
{
    return GraphType::kGraphPicture;
}

void GraphicGraph::onPictureSelected(const QString &file)
{
    if(!file.isEmpty()) {
        file_ = file;
        bool ret = origin_pixmap_.load(file);
        pixmap_ = origin_pixmap_.scaled(size_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if(!ret) {

        }
        canvas_->update();
    }
}
