#pragma once

#include <QObject>
#include "Item.h"
#include "Memo.h"

class ItemMemo : public Item
{
public:
    ItemMemo(Item* parent = nullptr);
    ~ItemMemo();

    virtual void paint(QPainter* p, const QRect& rect);
    virtual QSize sizeHint() const;
    virtual IType type() { return IType::Type_File; }

protected:
    virtual bool operator==(Item*) override;
private:
};
