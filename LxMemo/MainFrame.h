#pragma once

#include <QFrame>
#include <QList>
#include "Item.h"
#include "Memo.h"

enum class ShowStyle
{
    SS_LIST,
    SS_GRID,
};

class MainFrame : public QFrame
{
    Q_OBJECT

public:
    MainFrame(QWidget* parent = nullptr);
    ~MainFrame();

    Item* item(const QPoint& pos);
    Item* item(int index);
    int itemCount();

    void addItem(Item* item);
    void insertItem(int index, Item* item);
    void removeItem(int index);
    void removeItem(Item* item);
    void clear();

    void setShowStyle(ShowStyle style) { _style = style; }
    Item* getItemByPath(const QString& path);
    QString currentPath();
signals:
    void doubleClicked(Item*);
private slots:
    void onBackClicked();
protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* e) override;

private:
    ShowStyle _style = ShowStyle::SS_LIST;
    //QList<Item*> _items;
    Item* _root = nullptr;

    Item* _selItem = nullptr;
    int _selMode = -1;

    int _scrval = 0;

};
