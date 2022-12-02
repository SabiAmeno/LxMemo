#pragma once

#include <QRect>
#include <QColor>
#include <QByteArray>
#include <QPainter>
#include <QSharedPointer>

class Meta :public QEnableSharedFromThis<Meta>
{
public:
    virtual void paint(QPainter& painter, const QRect& rect) const = 0;
    virtual size_t type() = 0;

    template<typename T>
    QSharedPointer<T> converTo()
    {
        return qSharedPointerCast<T>(sharedFromThis());
    }

    void SetColor(const QColor& color) { color_ = color; }
    QColor GetColor() const { return color_; }

    QByteArray PropertyToStream();
    void StreamToProperty(const QByteArray& str);

    QString Time() { return time_; }
    void SetTime(const QString& time) { time_ = time; }

    uint32_t Id() const { return id_; }

protected:
    Meta();
    Meta(uint32_t id);

    uint32_t id_;
    QString time_;
    QColor color_{ 0XE3CCC4 };
};

uint32_t GenId();

using SharedMeta = QSharedPointer<Meta>;
Q_DECLARE_METATYPE(SharedMeta);
