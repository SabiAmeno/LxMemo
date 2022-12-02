#include "graphiccanvas.h"
#include "textgraph.h"
#include "graphicgraph.h"
#include <QClipboard>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMimeData>
#include <QBuffer>
#include <ctime>
#include <QMenu>

GraphicCanvas::GraphicCanvas(QWidget *parent) :
    QWidget(nullptr)
{
    ui.setupUi(this);

    group_ = new GraphGroup(this);

    connect(ui.word_button, &QToolButton::clicked, this, &GraphicCanvas::onWordAdd);
    connect(ui.picture_button, &QToolButton::clicked, this, &GraphicCanvas::onPictureAdd);
    connect(ui.graph_button, &QToolButton::clicked, this, &GraphicCanvas::onGraphAdd);
    setMouseTracking(true);
}

GraphicCanvas::~GraphicCanvas()
{
}

void GraphicCanvas::onWordAdd()
{
    TextGraph* tg = new TextGraph(this, "Hello World");
    srand(time(0));
    int x = rand() % width();
    int y = rand() % height();
    tg->SetCoord(QPoint(x, y));
    group_->AddGraph(tg);
    update();
}

void GraphicCanvas::onPictureAdd()
{
    GraphicGraph* tg = new GraphicGraph(this);
    srand(time(0));
    int x = rand() % width();
    int y = rand() % height();
    tg->SetCoord(QPoint(x, y));
    group_->AddGraph(tg);
    update();
}

void GraphicCanvas::onDeleteGraph()
{
    if(selected_graph_)
        group_->DeleteGraph(selected_graph_);
    update();
}

void GraphicCanvas::onGraphAdd()
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    group_->SerializeTo(stream);
    buffer.close();

//    qDebug() << ba;

    QBuffer rbuf(&ba);
    rbuf.open(QIODevice::ReadOnly);
    QDataStream rstream(&rbuf);

    group_->DeleteAll();

    GraphType type ;
    rstream >> type;

    if(GraphType::kGraphGroup == type) {
        if(!group_)
            group_ = new GraphGroup(this);
        group_->DeserializeFrom(rstream);
    }
    update();
}

void GraphicCanvas::onMoveUp()
{
    group_->MoveUp(selected_graph_);
    update();
}

void GraphicCanvas::onMoveDown()
{
    group_->MoveDown(selected_graph_);
    update();
}

void GraphicCanvas::onMoveTop()
{
    group_->MoveTop(selected_graph_);
    update();
}

void GraphicCanvas::onMoveBottom()
{
    group_->MoveBottom(selected_graph_);
    update();
}

void GraphicCanvas::onAddText()
{
    TextGraph* tg = new TextGraph(this, "Hello World");
    tg->SetCoord(mapFromGlobal(cursor().pos()));
    group_->AddGraph(tg);
    update();
}

void GraphicCanvas::onAddPicture()
{
    GraphicGraph* tg = new GraphicGraph(this);
    tg->SetCoord(mapFromGlobal(cursor().pos()));
    group_->AddGraph(tg);
    update();
}

void GraphicCanvas::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    group_->Draw(p);
}

void GraphicCanvas::mousePressEvent(QMouseEvent *e)
{
//    if(selected_graph_)
//        selected_graph_->SetSelected(false);

//    selected_graph_ = group_->At(e->pos());
//    if(selected_graph_)
//        selected_graph_->SetSelected(true);

    if(e->button() == Qt::LeftButton) {
        move_start_ = e->pos();
        if(selected_graph_ && cursor_direction_ != kDirectionNone) {

        } else {
            if(selected_graph_) {
                selected_graph_->SetSelected(false);
            }
            group_->MouseClick(e->pos());
            selected_graph_ = group_->SelectGraph();
            update();
        }
    } else {
        //右键也进行选取
        if(selected_graph_) {
            selected_graph_->SetSelected(false);
        }
        group_->MouseClick(e->pos());
        selected_graph_ = group_->SelectGraph();

        update();

        QMenu menu;
        if(selected_graph_) {
            auto del_action = menu.addAction("delete");
            auto move_down = menu.addAction("move down");
            auto move_up = menu.addAction("move up");
            auto move_top = menu.addAction("move top");
            auto move_bottom = menu.addAction("move bottom");

            connect(del_action, &QAction::triggered, this, &GraphicCanvas::onDeleteGraph);
            connect(move_down, &QAction::triggered, this, &GraphicCanvas::onMoveDown);
            connect(move_up, &QAction::triggered, this, &GraphicCanvas::onMoveUp);
            connect(move_top, &QAction::triggered, this, &GraphicCanvas::onMoveTop);
            connect(move_bottom, &QAction::triggered, this, &GraphicCanvas::onMoveBottom);
        } else {
            auto add_text_action = menu.addAction("add text");
            auto add_pix_action = menu.addAction("add picture");

            connect(add_text_action, &QAction::triggered, this, &GraphicCanvas::onAddText);
            connect(add_pix_action, &QAction::triggered, this, &GraphicCanvas::onAddPicture);
        }
        menu.exec(mapToGlobal(e->pos()));
    }
}

void GraphicCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if(selected_graph_) {
        ///移动graph
        if(!move_start_.isNull()) {
            auto pos = e->pos();
            auto oripos = selected_graph_->Coord();
            auto diff = pos - move_start_;
            move_start_ = pos;

            ///正常的拖动模式
            if(cursor_direction_ == kDirectionNone) {
                oripos += diff;
                selected_graph_->SetCoord(oripos);
            } else {
                //缩放graph
                switch (cursor_direction_) {
                case kDirectionTop:
                {
                    int diff_hg = pos.y() - oripos.y();
                    auto size = selected_graph_->Size();
                    oripos.setY(oripos.y() + diff_hg);
                    size.setHeight(size.height() - diff_hg);
                    if(size.height() > 5) {
                        selected_graph_->SetCoord(oripos);
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionBottom:{
//                    int diff_hg = pos.y() - oripos.y();
                    auto size = selected_graph_->Size();
                    size.setHeight(size.height() + diff.y());
                    if(size.height() > 5) {
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionLeft: {
                    int diff_hg = pos.x() - oripos.x();
                    auto size = selected_graph_->Size();
                    oripos.setX(oripos.x() + diff_hg);
                    size.setWidth(size.width() - diff_hg);
                    if(size.width() > 5) {
                        selected_graph_->SetCoord(oripos);
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionRight: {
                    auto size = selected_graph_->Size();
                    size.setWidth(size.width() + diff.x());
                    if(size.width() > 5) {
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionTopLeft:
                {
                    auto size = selected_graph_->Size();
                    oripos += diff;
                    size -= QSize(diff.x(), diff.y());
                    if(size.height() > 5) {
                        selected_graph_->SetCoord(oripos);
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionTopRight:
                {
                    auto size = selected_graph_->Size();
                    oripos.setY(oripos.y() + diff.y());
                    size += QSize(diff.x(), -diff.y());
                    if(size.width() > 5) {
                        selected_graph_->SetCoord(oripos);
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionBottomLeft:
                {
                    auto size = selected_graph_->Size();
                    oripos.setX(oripos.x() + diff.x());
                    size += QSize(-diff.x(), diff.y());
                    if(size.height() > 5) {
                        selected_graph_->SetCoord(oripos);
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                case kDirectionBottomRight:
                {
                    auto size = selected_graph_->Size();
//                    oripos.setY(oripos.y() + diff.y());
                    size += QSize(diff.x(), diff.y());
                    if(size.width() > 5) {
//                        selected_graph_->SetCoord(oripos);
                        selected_graph_->SetSize(size);
                    }
                    break;
                }
                default:
                    break;
                }
            }
        } else {
            ///显示缩放框
            auto geo = selected_graph_->Geometry();
            changeCursor(geo, e->pos());
        }
    }
    update();
}

void GraphicCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    move_start_ = QPoint();
}

void GraphicCanvas::wheelEvent(QWheelEvent *e)
{

}

void GraphicCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(!selected_graph_)
        selected_graph_ = group_->At(e->pos());

    if(selected_graph_)
        selected_graph_->MouseDoubleClick(e->pos());

    update();
}

void GraphicCanvas::keyPressEvent(QKeyEvent *e)
{
    QWidget::keyPressEvent(e);
}

void GraphicCanvas::keyReleaseEvent(QKeyEvent *e)
{
    auto key = e->key();
    if(Qt::Key_Delete == key) {
        if(selected_graph_)
            onDeleteGraph();
    }

    auto ctrl = e->modifiers();
    if(Qt::Key_V == key && Qt::ControlModifier == ctrl) {
        auto cboard = QGuiApplication::clipboard();
        auto meta = cboard->mimeData();

        auto pos = mapFromGlobal(cursor().pos());
        if(!rect().contains(pos))
            pos = rect().center();

        if(meta->hasImage()) {
            auto img_data = meta->imageData();
            QImage img = qvariant_cast<QImage>(img_data);
            QPixmap img0;
            img0.convertFromImage(img);
            GraphicGraph* tg = new GraphicGraph(this);
            tg->SetCoord(pos);
            tg->SetPicture(img0);
            group_->AddGraph(tg);
            update();
        } else if(meta->hasText() || meta->hasHtml()) {
            QString text;
            if(meta->hasHtml())
                text = QString::fromStdString(meta->html().toStdString());
            else
                text = meta->text();
            TextGraph* tg = new TextGraph(this, text);
            tg->SetCoord(pos);
            group_->AddGraph(tg);
            update();
        }
    }
}

void GraphicCanvas::changeCursor(const QRect &geo, const QPoint &p)
{
    auto left_rect = QRect(geo.topLeft() - QPoint(3, 0), QSize(6, geo.height()));
    auto right_rect = QRect(geo.topRight() - QPoint(3, 0), QSize(6, geo.height()));
    auto top_rect = QRect(geo.topLeft() - QPoint(0, 3), QSize(geo.width(), 6));
    auto bottom_rect = QRect(geo.bottomLeft() - QPoint(0, 3), QSize(geo.width(), 6));

    auto top_left_rect = QRect(geo.topLeft() - QPoint(3, 3), QSize(6, 6));
    auto top_right_rect = QRect(geo.topRight() - QPoint(3, 3), QSize(6,6));
    auto bottom_right_rect = QRect(geo.bottomRight() - QPoint(3, 3), QSize(6,6));
    auto bottom_left_rect = QRect(geo.bottomLeft() - QPoint(3,3), QSize(6,6));

    cursor_direction_ = kDirectionNone;

    if(top_left_rect.contains(p)) cursor_direction_ = kDirectionTopLeft;
    else if(top_right_rect.contains(p)) cursor_direction_ = kDirectionTopRight;
    else if(bottom_left_rect.contains(p)) cursor_direction_ = kDirectionBottomLeft;
    else if(bottom_right_rect.contains(p)) cursor_direction_ = kDirectionBottomRight;
    else if(top_rect.contains(p)) cursor_direction_ = kDirectionTop;
    else if(bottom_rect.contains(p)) cursor_direction_ = kDirectionBottom;
    else if(left_rect.contains(p)) cursor_direction_ = kDirectionLeft;
    else if(right_rect.contains(p)) cursor_direction_ = kDirectionRight;

    switch (cursor_direction_) {
    case kDirectionTop:
    case kDirectionBottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case kDirectionLeft:
    case kDirectionRight:
        setCursor(Qt::SizeHorCursor);
        break;
    case kDirectionTopLeft:
    case kDirectionBottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case kDirectionTopRight:
    case kDirectionBottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
        break;
    }

}
