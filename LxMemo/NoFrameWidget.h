#pragma once

#include <QWidget>
#include "ui_NoFrameWidget.h"

class NoFrameWidget : public QWidget
{
    Q_OBJECT

public:
    NoFrameWidget(QWidget *parent = nullptr);
    ~NoFrameWidget();

    void SetOpacity(qreal op);
    void SetPureStyle(const QColor& color);
protected:
    //virtual bool eventFilter(QObject* w, QEvent* e) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    virtual void showEvent(QShowEvent* event) override;

private:
    int shadeWidth = 9;

    QPoint pStart;
    bool   isMove{ false };
private:
    Ui::NoFrameWidgetClass ui;
};
