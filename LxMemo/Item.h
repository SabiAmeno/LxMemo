#pragma once

#include <QPaintDevice>
#include <QObject>
#include <QSize>
#include <QVariant>
#include <QList>

class Item;
using ItemList = QList<Item*>;

enum class IType
{
    Type_Folder,
    Type_File,
};

class Item
{
public:
    Item(Item* parent = nullptr);
    ~Item();

    void setParent(Item* parent) { _parent = parent; }
    void setData(int role, const QVariant&);
    QVariant data(int role) const;

    virtual void paint(QPainter*, const QRect&) = 0;
    virtual QSize sizeHint() const = 0;
    virtual IType type() = 0;

    void add(Item* item);
    void insert(int index, Item* item);
    void remove(Item* item);
    void remove(int index);
    void removeRecursivly();
    void clear();

    Item* parent() { return _parent; }
    ItemList children() { return _chrs; }

    ///通过名字获取对应的item
    Item* item(const QString& name);
protected:
    virtual bool operator==(Item*) = 0;
private:
    bool exist(Item* i);
private:
    QMap<int, QVariant> _vmap;

    Item* _parent = nullptr;
    ItemList _chrs;
};

Q_DECLARE_METATYPE(Item*);