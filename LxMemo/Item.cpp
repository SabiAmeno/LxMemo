#include "Item.h"

Item::Item(Item* parent)
    : _parent(parent)
{}

Item::~Item()
{}

void Item::setData(int role, const QVariant& v)
{
    _vmap.insert(role, v);
}

QVariant Item::data(int role) const
{
    return _vmap.value(role);
}

void Item::add(Item* item)
{
    if (!exist(item)) {
        item->setParent(this);
        _chrs.append(item);
    }
}

void Item::insert(int index, Item* item)
{
    if (!exist(item)) {
        item->setParent(this);
        _chrs.insert(index, item);
    }
}

void Item::remove(Item* item)
{
    _chrs.removeOne(item);
    delete item;
}

void Item::remove(int index)
{
    remove(_chrs.at(index));
}

void Item::removeRecursivly()
{
    for (auto& i : _chrs) {
        if (IType::Type_Folder == i->type())
            i->removeRecursivly();
        remove(i);
    }
}

void Item::clear()
{
}

Item* Item::item(const QString& name)
{
    for (auto& i : _chrs) {
        auto n = i->data(1);
        if (n.isValid() && n.toString().toLocal8Bit() == name.toLocal8Bit())
            return i;
    }

    return nullptr;
}

bool Item::exist(Item* i)
{
    for (const auto& itm : _chrs)
        if (itm == i)
            return true;

    return false;
}