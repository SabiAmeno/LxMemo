#ifndef NOTE_WIDGET_H
#define NOTE_WIDGET_H

#include <QColor>
#include <QPixmap>
#include <QPaintEvent>
#include "Meta.h"

//class Note;
class NoteWidget
{
public:
    explicit NoteWidget(SharedMeta meta = nullptr);
    ~NoteWidget();

    void SetMeta(SharedMeta meta) { meta_ = meta; }

    QSize Size() {return size_;}
    QPoint Position() {return posi_;}
    QRect Geometry() {return QRect(posi_, size_);}

    void SetSize(const QSize& size);
    void SetPosition(const QPoint& pos);
    void SetOffset(const QPoint& dpos);
    void SetBackground(const QColor& color);

    void Draw(QPainter &);
    void Repaint();

    SharedMeta MetaData() const { return meta_; }
private:
    void drawHeader(QPainter&);
    void drawContent(QPainter&);

private:
    QSize size_{64, 64};
    QPoint posi_{0,0};
    QColor background_{Qt::darkGreen};

    bool repaint_{true};
    QPixmap cache_;
private:
    //Note* note_{nullptr};
    SharedMeta meta_{ nullptr };
};

#endif // NOTE_H
