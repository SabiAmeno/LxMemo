#include "util.h"
#include <QPropertyAnimation>

template<typename T>
void AnimationImpl(QWidget* target, const T& from, const T& to, QEasingCurve easing, int duration, const QByteArray& prop)
{
    QPropertyAnimation* ani = new QPropertyAnimation(target, prop);
    ani->setStartValue(from);
    ani->setEndValue(to);
    ani->setEasingCurve(easing);
    ani->setDuration(duration);
    ani->start(QAbstractAnimation::DeleteWhenStopped);
}
//////////////////////ANIMATION FUNCTION//////////////////////////////
void StartOpacityAnimation(QWidget* target, double from, double to, QEasingCurve easing, int duration)
{
    AnimationImpl(target, from, to, easing, duration, "windowOpacity");
}
void StartGeometryAnimation(QWidget* target, const QRect& from, const QRect& to, QEasingCurve easing, int duration)
{
    AnimationImpl(target, from, to, easing, duration, "geometry");
}