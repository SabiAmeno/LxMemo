#include "notemanager.h"
#include "note.h"

NoteManager::NoteManager(QObject *parent)
    : QObject{parent}
{

}

NoteWidget *NoteManager::AddNote(int ind)
{
    NoteWidget* note = new NoteWidget();
    if (ind < 0)
        note_list_.append(note);
    else 
        note_list_.insert(ind, note);

    return note;
}

void NoteManager::DeleteNote(NoteWidget* note)
{
    if (note) {
        note_list_.removeAll(note);
    }
}

void NoteManager::WipeUp()
{
    note_list_.clear();
}

void NoteManager::DrawNotes(QPainter &dc)
{
    for(const auto& note: note_list_)
        note->Draw(dc);
}

NoteWidget *NoteManager::NoteIn(const QPoint &p)
{
    int ind = note_list_.size();
    for(auto r = note_list_.rbegin();
        r != note_list_.rend();
        ++r)
    {
        ind -= 1;
        if((*r)->Geometry().contains(p)) {
            /*auto node = *r;
            raiseUp(ind);
            return node;*/
            return *r;
        }
    }

    return nullptr;
}

void NoteManager::raiseUp(int index)
{
    note_list_.swapItemsAt(index, note_list_.size() - 1);
}
