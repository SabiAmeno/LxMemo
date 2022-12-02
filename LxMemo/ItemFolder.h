#pragma once

#include <QObject>
#include "Item.h"

class ItemFolder : public Item
{
public:
    ItemFolder(Item* parent = nullptr);
    ~ItemFolder();

    void addItem(Item* item);

    virtual void paint(QPainter* p, const QRect& rect);
    virtual QSize sizeHint() const;
    virtual IType type() { return IType::Type_Folder; }

protected:
    virtual bool operator==(Item*) override;
private:
    QList<Item*> _items;
};
