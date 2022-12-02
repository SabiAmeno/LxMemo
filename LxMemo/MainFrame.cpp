#include "MainFrame.h"
#include <QPainter>
#include <QMouseEvent>
#include <QToolButton>
#include <QScrollBar>
#include "ItemFolder.h"

MainFrame::MainFrame(QWidget* parent)
    : QFrame(parent)
{
    _root = new ItemFolder();

    setMouseTracking(true);

    QToolButton* back = new QToolButton(this);
    //back->move(20, 20);
    back->setAutoRaise(true);
    back->setIcon(QIcon(":/LxMemo/icons/back.png"));
    back->setFixedSize(24, 24);
    back->show();

    connect(back, &QToolButton::clicked, this, &MainFrame::onBackClicked);
}

MainFrame::~MainFrame()
{}

Item* MainFrame::item(const QPoint& pos)
{
    auto n = (pos.y() - 24) / 20;
    if (n >= _root->children().size() || n < 0)
        return nullptr;

    return item(n);
}

Item* MainFrame::item(int index)
{
    return _root->children().at(index);
}

int MainFrame::itemCount()
{
    return _root->children().size();
}

void MainFrame::addItem(Item* item)
{
    _root->add(item);

    update();
}

void MainFrame::insertItem(int index, Item* item)
{
    _root->insert(index, item);

    update();
}

void MainFrame::removeItem(int index)
{
    _root->remove(index);

    update();
}

void MainFrame::removeItem(Item* item)
{
    _root->remove(item);

    update();
}

void MainFrame::clear()
{
    _root->clear();
}

Item* MainFrame::getItemByPath(const QString& path)
{
    auto root = _root;
    while (root->parent()) root = root->parent();

    if (path.isEmpty())
        return root;

    auto tp = path;
    auto pathes = tp.split('/', Qt::SkipEmptyParts);

    for (auto& p : pathes) {
        auto i = root->item(p);
        if (!i) {
            i = new ItemFolder(root);
            i->setData(1, p);
            root->add(i);
        }

        root = i;
    }

    return root;
}

QString MainFrame::currentPath()
{
    auto root = _root;
    QString path;

    while (root) {
        auto name = root->data(1).toString();
        path = name + "/" + path;
        root = root->parent();
    }
    return path;
}

void MainFrame::onBackClicked()
{
    if (_root->parent())
        _root = _root->parent();

    update();
}

void MainFrame::paintEvent(QPaintEvent* e)
{
    int dx = 3, dy = 24;
    int w = width(), h = height();

    QPainter p(this);
    QRect rct(dx, dy, w - 18, 20);
    p.drawRect(0, 0, width() - 18, height() - 24);

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    for (auto i : _root->children())
    {
        if (i == _selItem) {
            QColor color(0, 0, 255, 50);
            if (_selMode == 1)
                color.setAlpha(150);
            p.fillRect(rct, color);
        }
        i->paint(&p, rct);
        rct.moveTo(dx, rct.bottom());
    }

    p.fillRect(width() - 10, 0, 8, height() - 24, qRgba(160, 160, 160, 60));



    QFrame::paintEvent(e);
}

void MainFrame::mouseMoveEvent(QMouseEvent* e)
{
    auto pos = e->pos();
    if (pos.x() < width() - 18) {
        _selItem = item(pos);
        _selMode = 0;

        update();
    }

    QFrame::mouseMoveEvent(e);
}

void MainFrame::mousePressEvent(QMouseEvent* e)
{
    auto pos = e->pos();
    if ( pos.x() < width() - 18) {
        _selItem = item(pos);
        _selMode = 1;

        update();
    }

    QFrame::mousePressEvent(e);
}

void MainFrame::mouseReleaseEvent(QMouseEvent* e)
{
    update();
    QFrame::mouseReleaseEvent(e);
}

void MainFrame::mouseDoubleClickEvent(QMouseEvent* e)
{
    auto i = item(e->pos());
    if (i && IType::Type_Folder == i->type())
        _root = i;
    if (i && IType::Type_File == i->type())
        emit doubleClicked(i);

    update();

    QFrame::mouseDoubleClickEvent(e);
}