#include "AnimationSW.h"

#include <QPixmap>
#include <QPainter>
#include <QTransform>
#include <QPropertyAnimation>

AnimationSW::AnimationSW(QWidget* parent)
    : QStackedWidget(parent)
{
    _animation = new QPropertyAnimation(this, QByteArray());
    _animation->setDuration(200);
    _animation->setEasingCurve(QEasingCurve::Linear);
    _animation->setStartValue(-100);
    _animation->setEndValue(100);

    connect(_animation, SIGNAL(valueChanged(QVariant)), SLOT(valueChanged(QVariant)));
    connect(_animation, SIGNAL(finished()), SLOT(animationFinished()));
}

AnimationSW::~AnimationSW()
{
    delete _animation;
}

void AnimationSW::paintEvent(QPaintEvent* event)
{
    if (_isAnimating)
    {
        QPainter painter(this);
        QTransform transform;
        renderCurrentWidget(painter, transform);
        renderPreviousWidget(painter, transform);
    }

    QWidget::paintEvent(event);
}

void AnimationSW::renderPreviousWidget(QPainter& painter, QTransform& transform)
{
    QWidget* w = widget(_previousIndex);
    QPixmap pixmap(w->size());
    pixmap.fill(w->palette().brush(QPalette::Background).color());
    w->render(&pixmap);

    painter.drawPixmap(width() / 2, 0, pixmap);
}

void AnimationSW::renderCurrentWidget(QPainter& painter, QTransform& transform)
{
    QWidget* w = widget(_currentIndex);
    QPixmap pixmap(w->size());
    pixmap.fill(w->palette().brush(QPalette::Background).color());
    w->render(&pixmap);

    transform.translate(_currentValue, 0);
    painter.setTransform(transform);
    painter.drawPixmap(-width() / 2, 0, pixmap);
}

void AnimationSW::start(int index)
{
    if (_isAnimating)
        return;
    _previousIndex = _currentIndex;
    _currentIndex = index;

    QWidget* w = widget(_currentIndex);
    if (!w)
        return;

    const int offsetx = frameRect().width();
    const int offsety = frameRect().height();
    w->setGeometry(0, 0, offsetx, offsety);

    currentWidget()->hide();
    _isAnimating = true;
    _animation->start();
}

void AnimationSW::SetCurrentIndex(int index)
{
    setCurrentIndex(index);

    if (_isAnimating)
        return;
    _previousIndex = _currentIndex;
    _currentIndex = index;

    QWidget* w = widget(_currentIndex);
    if (!w)
        return;

    const int offsetx = frameRect().width();
    const int offsety = frameRect().height();
    w->setGeometry(0, 0, offsetx, offsety);

    currentWidget()->hide();
    _isAnimating = true;
    _animation->start();
}

void AnimationSW::valueChanged(const QVariant& value)
{
    _currentValue = value.toFloat();
    update();
}

void AnimationSW::animationFinished()
{
    _currentValue = 0;
    _isAnimating = false;

    QWidget* w = widget(_currentIndex);
    if (w != nullptr) {
        w->show();
        w->raise();
        QStackedWidget::setCurrentWidget(w);
    }

    update();
}