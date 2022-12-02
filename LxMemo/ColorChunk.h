#pragma once

#include <QWidget>

class ColorChunk : public QWidget
{
    Q_OBJECT

public:
    ColorChunk(QWidget* parent = nullptr);
    ~ColorChunk();

    void AddColor(const QColor& color);

signals:
    void ColorSelected(const QColor&);
protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
private:
    QVector<QColor> _colorList;
};
