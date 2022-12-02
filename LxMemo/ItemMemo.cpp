#include "ItemMemo.h"
#include <QPalette>

ItemMemo::ItemMemo(Item* parent)
    : Item(parent)
{}

ItemMemo::~ItemMemo()
{}

void ItemMemo::paint(QPainter* p, const QRect& rect)
{
    auto memo = qvariant_cast<SharedMemo>(data(0));
    if (memo)
        memo->paint(p, rect, QPalette(), Memo::EditMode::ReadOnly);
}

QSize ItemMemo::sizeHint() const
{
    auto memo = qvariant_cast<SharedMemo>(data(0));
    if (memo)
        return memo->sizeHint();

    return QSize();
}

bool ItemMemo::operator==(Item* i)
{
    if (!i) return false;

    auto d1 = data(0);
    auto d2 = i->data(0);

    return d1 == d2;
}