#include "Meta.h"
#include <random>
#include <QDataStream>

QByteArray Meta::PropertyToStream()
{
    QByteArray dev;
    QDataStream ds(&dev, QIODevice::WriteOnly);
    ds << color_;

    return dev.toBase64();
}

void Meta::StreamToProperty(const QByteArray& str)
{
    auto array = QByteArray::fromBase64(str);
    QDataStream ds(&array, QIODevice::ReadOnly);
    ds >> color_;
}

Meta::Meta()
    : id_(GenId())
{
}

Meta::Meta(uint32_t id)
    : id_(id)
{
}

uint32_t GenId()
{
    static uint32_t MASK = 0xFFFFFFFF;
    auto now = time(nullptr);

    uint32_t d = now;
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<uint32_t> uniform_dist(0X10000000, 0XFFFFFFFF);
    uint32_t mean = uniform_dist(e1);

    std::seed_seq seed2{ r(), r(), r(), r(), r(), r(), r(), r() };
    std::mt19937 e2(seed2);
    std::normal_distribution<> normal_dist(mean, 8);

    for (int i = 0; i < 8; i++) {
        uint32_t r = std::round(normal_dist(e2));
        d |= (r << 16) | (r >> 16);
        d &= ((d + now) & uint32_t(0xFFFF) << 16) | ((d + now) >> 16) & 0xFFFF;
    }

    //printf("id=%u\n", d);
    return d;
}