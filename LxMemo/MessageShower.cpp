#include "MessageShower.h"
#include <QHBoxLayout>
#include <QBitMap>
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include "util.h"

MessageShower::MessageShower(QWidget* parent)
    : QWidget(parent)
{
    _timer.setSingleShot(true);
    _timer.setInterval(2000);

    _timer.start();

    connect(&_timer, &QTimer::timeout, this, &MessageShower::onTimeout);
}

MessageShower::~MessageShower()
{}

void MessageShower::setText(const QString& msg)
{
    _msg = msg;
    //_msg->setText(msg);
}

void MessageShower::Start(const QString& msg, QWidget* parent)
{
    MessageShower* ms = new MessageShower(parent);
    ms->setAttribute(Qt::WA_DeleteOnClose);
    ms->setText(msg);
    ms->show();

    if (parent) {
        auto sz = parent->size();
        StartGeometryAnimation(ms, QRect(0, sz.height(), sz.width(), 0), QRect(0, sz.height() - 55, sz.width(), 55));
    }
}

void MessageShower::paintEvent(QPaintEvent* e)
{
    QPainter p(this);

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QFont font("Microsoft YaHei UI", 12);

    p.save();
    QPainterPath path;
    path.addRoundedRect(QRect(0, 0, width(), 50), 5, 5);
    p.fillPath(path, QColor("#665dad63"));
    p.restore();

    auto pix = QPixmap(":/LxMemo/icons/red_mark.png")/*.scaled(16, 16)*/;
    p.drawPixmap(6, height() / 2 - 8, pix);

    p.save();
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);
    p.setPen(pen);
    p.setFont(font);

    p.drawText(28, 33, _msg);
    p.restore();

    QWidget::paintEvent(e);
}

void MessageShower::onTimeout()
{
    close();
}