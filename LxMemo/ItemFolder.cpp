#include "ItemFolder.h"
#include <QPainter>

ItemFolder::ItemFolder(Item* parent)
    : Item(parent)
{}

ItemFolder::~ItemFolder()
{}

void ItemFolder::addItem(Item* item)
{
    _items.append(item);
}

void ItemFolder::paint(QPainter* p, const QRect& rect)
{
    int hi = rect.height();

    QPixmap pix(":/LxMemo/icons/mini-folder.png");
    pix = pix.scaled(hi, hi);

    //p->fillRect(rect, QColor(0, 0, 100, 140));
    p->drawPixmap(rect.left(), rect.top(), hi, hi, pix);

    auto name = data(1).toString();
    p->drawText(rect.left() + 25, rect.top() + 14, name);
}

QSize ItemFolder::sizeHint() const
{
    return QSize(100, 100);
}

bool ItemFolder::operator==(Item* i)
{
    auto name0 = data(1).toString();
    auto name1 = i->data(1).toString();

    return name0.toLocal8Bit() == name1.toLocal8Bit();
}