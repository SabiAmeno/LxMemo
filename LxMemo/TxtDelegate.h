#pragma once

#include <QObject>
#include <QStyledItemDelegate>

class TxtDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

signals:
    void editFinshed(uint32_t id, const QString& last, const QString& curr);
private slots:
    void onEditFinished(const QString&, const QString&);

private:
    mutable QModelIndex _editIndex;
};
