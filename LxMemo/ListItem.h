#pragma once

#include <QLineEdit>
#include <QString>
#include "Memo.h"

class ListItem : public QLineEdit
{
    Q_OBJECT

public:
    ListItem(QWidget* parent);
    ~ListItem();

    QSize sizeHint() const override;

    void SetMeta(SharedMeta meta);
    SharedMeta GetMeta() const { return meta; }

signals:
    void editFinished(const QString& last, const QString& curr);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent*) override;

private:
    SharedMeta meta;
};