#include "note.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

NoteWidget::NoteWidget(SharedMeta meta)
    : meta_{meta}
{

}

NoteWidget::~NoteWidget()
{
}

void NoteWidget::SetSize(const QSize &size)
{
    size_ = size;
    Repaint();
}

void NoteWidget::SetPosition(const QPoint &pos)
{
    posi_ = pos;
    //Repaint();
}

void NoteWidget::SetOffset(const QPoint &dpos)
{
    posi_ += dpos;
    Repaint();
}

void NoteWidget::SetBackground(const QColor &color)
{
    background_ = color;
    Repaint();
}

void NoteWidget::Draw(QPainter& painter)
{
    if(repaint_)
    {
        repaint_ = false;
        cache_ = QPixmap(size_);
        cache_.fill(Qt::transparent);
        QPainter p(&cache_);
        p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//        p.fillRect(QRect(QPoint(0, 20), size_), background_);
        /*p.drawPixmap(0, 0, QPixmap(":/LxMemo/icons/note.png").scaled(size_, Qt::KeepAspectRatio, Qt::SmoothTransformation));*/
        //drawHeader(p);
        //drawContent(p);
        meta_->paint(p, QRect(QPoint(0, 0), size_));
    }
//    QPainter p(dc);
    painter.drawPixmap(QRect(posi_, size_), cache_);
//    p.setRenderHints(QPainter::Antialiasing);

//    auto pat = palette();
//    auto color = pat.color(QPalette::Window);

//    QPainterPath path;
//    path.addRoundedRect(rect(), 5, 5);
//    p.fillPath(path, color);
}

void NoteWidget::drawHeader(QPainter& painter)
{

}

void NoteWidget::drawContent(QPainter& painter)
{
    QString test = R"(寻寻觅觅，冷冷清清，凄凄惨惨戚戚。/n乍暖还寒时候，最难将息。/n三杯两盏淡酒，怎敌他、晚来风急！/n雁过也，正伤心，却是旧时相识。
满地黄花堆积，憔悴损，如今有谁堪摘？/n守着窗儿，独自怎生得黑！/n梧桐更兼细雨，到黄昏、点点滴滴。/n这次第，怎一个愁字了得！)";
    QFont font = painter.font();
    font.setPixelSize(10);
    painter.setFont(font);

    painter.save();
//    painter.rotate(-6);

    QFontMetrics fmt = painter.fontMetrics();
//    QSize sz = fmt.size(Qt::TextSingleLine, test);
    int h = fmt.height();

    int i = 0, init = 20;
    int y = h + init;
    auto texts = test.split("/n");
    for(const auto& t : texts) {
        painter.drawText(i * 2 + 10, y, t);
        i++;
        y += h;
        y += 5;
    }

    painter.restore();
//    painter.drawRect(0,0,size_.width(), size_.height());
}

void NoteWidget::Repaint()
{
    repaint_ = true;
}
