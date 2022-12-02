#include "TxtDelegate.h"
#include "ListItem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QLineEdit>

void TxtDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!_editIndex.isValid() || _editIndex != index) {
        auto meta = qvariant_cast<SharedMeta>(index.data());

        auto rect = option.rect;
        rect.adjust(4, 2, -4, -8);

        //dst.setBottom(rect.top() + 20);

        bool ok = false;
        int slide = index.data(Qt::UserRole + 1).toInt(&ok);
        /*if (slide < 0) {
            if (slide < -50) slide = -100;
            QPainterPath path;
            auto crct = rect;
            rect.adjust(0, 0, slide, 0);
            path.addRoundedRect(rect, 5, 5);
            painter->save();
            painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            if (option.state & QStyle::State_Selected)
                painter->fillPath(path, meta.GetColor().lighter(130));
            else
                painter->fillPath(path, meta.GetColor());

            QRect delRegin = QRect(rect.topRight(), crct.bottomRight());
            painter->fillRect(delRegin, Qt::red);

            QPixmap pix(":/LxMemo/icons/delete.png");
            int x = delRegin.center().x() - pix.width() / 2;
            int y = delRegin.center().y() - pix.height() / 2;
            painter->drawPixmap(QRect(QPoint(x, y), pix.size()), pix);

            //painter->drawPixmap(rect.topRight() - QPoint(25, -5), QPixmap(":/LxMemo/icons/other.png"));
            rect.adjust(0, 0, -10, 0);
            meta.paint(painter, rect, option.palette);
            painter->restore();
        }*/

        if (slide >= 0) {
            QPainterPath path;
            path.addRoundedRect(rect, 5, 5);
            painter->save();
            painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            painter->fillPath(path, meta->GetColor());

            if (option.state & QStyle::State_MouseOver)
                painter->fillPath(path, QColor(255, 255, 255, 80));
            //else
            //painter->fillPath(path, meta.GetColor());

            //painter->drawPixmap(rect.topRight() - QPoint(25, -5), QPixmap(":/LxMemo/icons/other.png"));
            /*rect.adjust(0, 0, -30, 0);*/
            meta->paint(*painter, rect);
            painter->restore();
        }
    }

    //return QStyledItemDelegate::paint(painter, option, index);
}

QSize TxtDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::sizeHint(option, index);
    auto meta = qvariant_cast<SharedMeta>(index.data());
    return meta->sizeHint();
}

QWidget* TxtDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto editor = new ListItem(parent);
    _editIndex = index;
    editor->setFixedHeight(40);
    connect(editor, &ListItem::editFinished, this, &TxtDelegate::onEditFinished);
    return editor;
}

void TxtDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto meta = qvariant_cast<SharedMeta>(index.data());
    auto item = qobject_cast<ListItem*>(editor);
    item->SetMeta(meta);
}

void TxtDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto item = qobject_cast<ListItem*>(editor);
    model->setData(index, QVariant::fromValue(item->GetMeta()));
}

void TxtDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}

void TxtDelegate::onEditFinished(const QString& last, const QString& curr)
{
    _editIndex = QModelIndex();
    auto editor = qobject_cast<ListItem*>(sender());

    emit commitData(editor);
    emit closeEditor(editor);

    if (last != curr)
        emit editFinshed(editor->GetMeta()->Id(), last, curr);
}