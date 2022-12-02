#pragma once

#include <QWidget>
#include "ui_ColorSelector.h"

///自定义颜色设置面板
class FaceColor : public QWidget
{
    Q_OBJECT
public:
    FaceColor(QWidget* parent = nullptr);

signals:
    void colorChanged(const QColor&);
protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void paint(QPaintDevice* pd) = 0;
};

class ColorSelector : public QWidget
{
    Q_OBJECT

public:
    ColorSelector(QWidget *parent = nullptr);
    ~ColorSelector();

    void SetColor(const QColor& color);
signals:
    void colorChanged(const QColor&);
private slots:
    void onColorChanged(int val);

protected:
    virtual void focusInEvent(QFocusEvent* event) override;
private:
    Ui::ColorSelector ui;
};


class ColorGrid : public FaceColor
{
    Q_OBJECT
public:
    ColorGrid(QWidget* parent)
        : FaceColor(parent)
    {}

protected:
    virtual void paint(QPaintDevice* pd) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual QSize sizeHint() const override;

private:
    int _selM = -1;
    int _selN = -1;

    QPoint _mousePos;
};


class ColorStrip : public FaceColor
{
    Q_OBJECT
public:
    ColorStrip(QWidget* parent)
        : FaceColor(parent)
    {}

protected:
    virtual void paint(QPaintDevice* pd) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual QSize sizeHint() const override;
};