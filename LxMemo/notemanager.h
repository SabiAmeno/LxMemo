#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include <QObject>
#include <QList>
#include <QPainter>

class NoteWidget;
using NoteList = QList<NoteWidget*>;

class NoteManager : public QObject
{
    Q_OBJECT
public:
    explicit NoteManager(QObject *parent = nullptr);

    NoteWidget *AddNote(int ind = -1);
    void DeleteNote(NoteWidget* note);
    ///Çå¿ÕËùÓÐnotes
    void WipeUp();

    int NoteCount() {return note_list_.size();}
    NoteList& Notes() {return note_list_;}
    void DrawNotes(QPainter& dc);
    NoteWidget* NoteIn(const QPoint& p);
private:
    void raiseUp(int index);
private:
    NoteList note_list_{};
};

#endif // NOTEMANAGER_H
