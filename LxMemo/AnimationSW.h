#pragma once

#include <QStackedWidget>
//#include "ttkglobaldefine.h"

class QPropertyAnimation;

class AnimationSW : public QStackedWidget
{
    Q_OBJECT
public:
    explicit AnimationSW(QWidget* parent = nullptr);
    ~AnimationSW();

    void start(int index);
    void SetCurrentIndex(int index);
private slots:
    void valueChanged(const QVariant& value);
    void animationFinished();

protected:
    virtual void paintEvent(QPaintEvent* event) override final;
    void renderPreviousWidget(QPainter& painter, QTransform& transform);
    void renderCurrentWidget(QPainter& painter, QTransform& transform);
private:
    bool _isAnimating{ false };
    float _currentValue{ 0 };
    int _currentIndex{ 0 };
    int _previousIndex{ 0 };

    QPropertyAnimation* _animation;
};
