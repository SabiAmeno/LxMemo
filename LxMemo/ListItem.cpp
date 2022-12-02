#include "ListItem.h"
#include <QKeyEvent>
#include "Folder.h"

ListItem::ListItem(QWidget* parent)
    : QLineEdit(parent)
{
    auto style = R"(
        QLineEdit {
             border: 1px solid gray;
             border-radius: 4px;
             padding: 0 8px;
             background: #ebebeb;
             selection-background-color: darkgray;
         }
    )";
    setStyleSheet(style);
}

ListItem::~ListItem()
{}

QSize ListItem::sizeHint() const
{
    return QSize(100, 40);
}

void ListItem::SetMeta(SharedMeta meta)
{
    this->meta = meta;
    auto f = qSharedPointerCast<Folder>(meta);
    setText(f->name());
}

void ListItem::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    //data->paint(painter.device(), rect());
    return QLineEdit::paintEvent(event);
}

void ListItem::mouseMoveEvent(QMouseEvent* event)
{
    return QLineEdit::mouseMoveEvent(event);
}

void ListItem::mouseReleaseEvent(QMouseEvent* event)
{
    return QLineEdit::mouseReleaseEvent(event);
}

void ListItem::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return
        || event->key() == Qt::Key_Enter)
    {
        auto folder = qSharedPointerCast<Folder>(meta);
        auto name = folder->name();

        if (!text().isEmpty()) {
            folder->setName(text());
        }
        emit editFinished(name, folder->name());
    }

    return QLineEdit::keyPressEvent(event);
}

void ListItem::focusOutEvent(QFocusEvent* event)
{
    auto folder = qSharedPointerCast<Folder>(meta);
    auto name = folder->name();

    if (!text().isEmpty()) {
        folder->setName(text());
    }
    emit editFinished(name, folder->name());

    return QLineEdit::focusOutEvent(event);
}