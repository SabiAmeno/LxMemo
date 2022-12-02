#pragma once

#include <QToolButton>
#include "lucencydialog.h"
#include "ColorSelector.h"

class ColorButton : public QToolButton
{
    Q_OBJECT

public:
    ColorButton(QWidget* parent);
    ~ColorButton();

    void SetColor(const QColor& color);
    void SetIcon(const QString& icon) { _icon = icon; }
    QColor Color() { return _color; }

    void SetColorFace(FaceColor* fc);
signals:
    void colorChnaged(const QColor&);
private slots:
    void onColorChanged(const QColor&);
protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    QColor _color = Qt::black;
    QString _icon;

    ColorSelector* selector;
};
