#ifndef NOTEWALL_H
#define NOTEWALL_H

#include <QWidget>
#include "notemanager.h"
#include "Memo.h"

namespace Ui {
class NoteWall;
}

class NoteWall : public QWidget
{
    Q_OBJECT

public:
    explicit NoteWall(QWidget *parent = nullptr);
    ~NoteWall();

    void Translate(int dx);
    NoteWidget* AddNote();
    void DeleteNote(NoteWidget* note);
    void WipeUp();
    void SetBackground(const QBrush& brush);
signals:
    //鼠标左键单击
    void NoteClicked(const QPoint&, NoteWidget*);
    //鼠标右键单击
    void NoteDoubleClicked(const QPoint&, NoteWidget*);
    //右键点击,如果没有选中note，则NoteWidget为空
    void CustomClicked(const QPoint&, NoteWidget*);

    void MemoAddTriggered(MemoType);
    void ImageAddTriggered();
    void FolderAddTriggered();
    void MemoImportTriggered();
public slots:
    void onNoteAdded();
protected:
    void paintEvent(QPaintEvent*) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* e) override;
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    virtual void resizeEvent(QResizeEvent* e) override;
    virtual void wheelEvent(QWheelEvent* e) override;
private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void onAdded();
private:
    void drawFrame(QPainter& painter, const QRect& geo, const QColor& color);
    void changeScrollRange();
    QPoint realCoord(const QPoint& pos);
private:
    Ui::NoteWall *ui;

    NoteManager* nm_;

//    bool repaint_{true};
    NoteWidget* hover_note_{nullptr};
    QPoint mouse_pressed_{};
//    QPixmap cache_{};

    //QColor background_{ Qt::darkGray };
    QPixmap background_{};
    int offset_y_{0};
    NoteWidget* selected_{nullptr};
};

#endif // NOTEWALL_H
