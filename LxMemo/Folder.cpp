#include "Folder.h"
#include <QPainter>
#include <QFontMetrics>

Folder::Folder()
    : Meta()
{
}

Folder::Folder(uint32_t id)
    : Meta(id)
{
}

QStringList NameSplit(const QString& name, const QFontMetrics& metrics, int width)
{
    int w = metrics.width(name, Qt::TextSingleLine);
    QStringList list;

    int rwidth = width - 5, cur_width = 0;
    QString s;
    for (auto c : name) {
        int cw = metrics.width(c);
        if (cur_width + cw < rwidth) {
            s += c;
            cur_width += cw;
        }
        else {
            list.append(s);
            s.clear();
            s += c;
            cur_width = 0;
        }
    }
    if (!s.isEmpty())
        list.append(s);

    return list;
}

void Folder::paint(QPainter& painter, const QRect& rect) const
{
    QPixmap pix(":/LxMemo/icons/folder.png");
    painter.drawPixmap(rect, pix.scaled(rect.size(), Qt::KeepAspectRatio));

    painter.save();

    QFont font("Microsoft YaHei UI", 10);
    painter.setFont(font);

    auto fmt = painter.fontMetrics();
    //int len = fmt.width(name_);
    auto ns = NameSplit(name_, fmt, rect.width());
    int x = rect.center().x(), y = rect.center().y();
    ///名称过长，只显示三行
    if (ns.size() > 3)
        ns.erase(ns.begin() + 3, ns.end());

    for (auto s : ns) {
        painter.drawText(x - fmt.width(s) / 2, y, s);
        y += 14;
    }

    /*QPen pen;
    pen.setColor(Qt::darkGreen);
    painter.setPen(pen);
    font.setPointSize(4);
    painter.setFont(font);
    painter.drawText(rect.left() + 8, rect.bottom() - 6, time_);*/

    painter.restore();
}

size_t Folder::type()
{
    return typeid(Folder).hash_code();
}
