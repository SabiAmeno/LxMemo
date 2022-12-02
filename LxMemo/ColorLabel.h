#pragma once

#include <QWidget>

class ColorLabel  : public QWidget
{
    Q_OBJECT

public:
    ColorLabel(QWidget *parent);
    ~ColorLabel();

    void SetColor(const QColor& color);
protected:
    virtual void paintEvent(QPaintEvent* event) override;
private:
    QColor _color;
};
