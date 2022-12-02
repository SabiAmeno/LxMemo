#include "notewall.h"
#include "ui_notewall.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include "note.h"

NoteWall::NoteWall(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteWall)
{
    ui->setupUi(this);

    background_.load(":/texture/icons/texture/race.jpg");
    nm_ = new NoteManager(this);
    setMouseTracking(true);
}

NoteWall::~NoteWall()
{
    delete ui;
}

void NoteWall::Translate(int dx)
{
    offset_y_ = dx;
    update();
}

NoteWidget* NoteWall::AddNote()
{
    int count = nm_->NoteCount();
    int r = count / 2;
    int c = count % 2;

    auto note = nm_->AddNote(0);
    //note->SetPosition(QPoint(30 + c * (94 + 20), 30 + r * (92 + 30)));

    changeScrollRange();
    update();

    return note;
}

void NoteWall::DeleteNote(NoteWidget* note)
{
    nm_->DeleteNote(note);

    changeScrollRange();
    repaint();
}

void NoteWall::WipeUp()
{
    nm_->WipeUp();
    hover_note_ = nullptr;
    selected_ = nullptr;

    changeScrollRange();
    repaint();
}

void NoteWall::SetBackground(const QBrush& brush)
{
    auto pal = palette();
    pal.setColor(QPalette::Window, brush.color());
    setPalette(pal);
    update();
}

void NoteWall::onNoteAdded()
{
    int count = nm_->NoteCount();
    int r = count / 2;
    int c = count % 2;

    auto note = nm_->AddNote();
    //note->SetPosition(QPoint(30 + c * (94 + 20), 30 + r * (92 + 30)));
    changeScrollRange();
    update();
}

void NoteWall::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    //p.drawPixmap(rect(), background_);
    p.translate(0, offset_y_);

    if(hover_note_) {
        QRect geo = hover_note_->Geometry();
        geo.adjust(-2, -2, 2, 2);

        drawFrame(p, geo, QColor("#bbc2f488"));
    }
    if(selected_) {
        QRect geo = selected_->Geometry();
        geo.adjust(-2, -2, 2, 2);

        drawFrame(p, geo, QColor("#bba1d9ff"));
    }

    auto& notes = nm_->Notes();
    int wd = width(), hg = height();

    int n = (wd - 40) / 114;
    int x = 30, y = 30;

    int row = notes.size() / n;
    int col = notes.size() % n;
    if (col != 0) row++;

    for (int r = 0; r < row; r++) {
        x = 30;
        for (int c = 0; c < n; c++) {
            int ind = r * n + c;
            if (ind < notes.size()) {
                auto note = notes.at(ind);
                note->SetPosition(QPoint(x, y));
                note->Draw(p);
            }
            x += 94 + 30;
        }
        y += 92 + 20;
    }

    QWidget::paintEvent(e);
}

void NoteWall::drawFrame(QPainter &painter, const QRect &geo, const QColor &color)
{
    painter.save();
    QPainterPath path;
    path.addRoundedRect(geo, 6, 6);
    painter.fillPath(path, color);
    painter.restore();
}

void NoteWall::mouseDoubleClickEvent(QMouseEvent *e)
{
    selected_ = nm_->NoteIn(realCoord(e->pos()));
    emit NoteDoubleClicked(e->pos(), selected_);
    update();
}

void NoteWall::mousePressEvent(QMouseEvent *e)
{
    mouse_pressed_ = realCoord(e->pos());
    selected_ = nm_->NoteIn(mouse_pressed_);
    hover_note_ = nullptr;

    switch (e->button()) {
        case Qt::LeftButton: 
            emit NoteClicked(e->pos(), selected_);
            break;
        case Qt::RightButton:
            emit CustomClicked(e->pos(), selected_);
            break;
        default:break;
    }

    update();
}

void NoteWall::mouseMoveEvent(QMouseEvent *e)
{
    if(!mouse_pressed_.isNull()) {
        if(selected_) {
//        selected_->SetOffset(e->pos() - move_start_);
//        move_start_ = e->pos();
        }
    } else {
        hover_note_ = nm_->NoteIn(realCoord(e->pos()));
        if(hover_note_)
            update();
    }
}

void NoteWall::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_pressed_ = QPoint{};
//    selected_ = nullptr;
}

void NoteWall::resizeEvent(QResizeEvent *e)
{
//    ui->verticalScrollBar->setMaximum(e->size().height());
//    repaint_ = true;
    changeScrollRange();
    QWidget::resizeEvent(e);
}

void NoteWall::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        Translate(-numPixels.y());
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15 * 40;
        int origin = ui->verticalScrollBar->value();
        ui->verticalScrollBar->setValue(origin - numSteps.y());
    }

    event->accept();
}

void NoteWall::on_verticalScrollBar_valueChanged(int value)
{
    Translate(-value);
}

void NoteWall::changeScrollRange()
{
    int count = nm_->NoteCount()-1;
    int r = count / 2 + 1;

    int h = 30 + r * (92 + 30);
    if(h > height()) {
        ui->verticalScrollBar->setMaximum(h - height());
//        ui->verticalScrollBar->setVisible(true);
    } else {
        ui->verticalScrollBar->setVisible(false);
        ui->verticalScrollBar->setMaximum(0);
    }
}

QPoint NoteWall::realCoord(const QPoint &pos)
{
    return pos - QPoint(0, offset_y_);
}

