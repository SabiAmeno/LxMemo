#pragma once

#include <QWidget>
#include <QEasingCurve>

void StartOpacityAnimation(
    QWidget* target,
    double from,
    double to,
    QEasingCurve easing = QEasingCurve::Linear,
    int duration = 200);

void StartGeometryAnimation(
    QWidget* target,
    const QRect& from,
    const QRect& to,
    QEasingCurve easing = QEasingCurve::Linear,
    int duration = 200);
