#include "ColorSelector.h"
#include <QPainter>
#include <QMouseEvent>

ColorSelector::ColorSelector(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.slider_R, &QSlider::valueChanged, this, &ColorSelector::onColorChanged);
    connect(ui.slider_G, &QSlider::valueChanged, this, &ColorSelector::onColorChanged);
    connect(ui.slider_B, &QSlider::valueChanged, this, &ColorSelector::onColorChanged);
}

ColorSelector::~ColorSelector()
{}

void ColorSelector::SetColor(const QColor & color)
{
    ui.slider_R->setValue(color.red());
    ui.slider_G->setValue(color.green());
    ui.slider_B->setValue(color.blue());

    update();
}

void ColorSelector::focusInEvent(QFocusEvent* event)
{
    //ui.slider_R->setFocus();

    QWidget::focusInEvent(event);
}

void ColorSelector::onColorChanged(int)
{
    int r = ui.slider_R->value();
    int g = ui.slider_G->value();
    int b = ui.slider_B->value();

    QColor color(r, g, b);
    ui.color_label->SetColor(color);

    emit colorChanged(color);
}

///////////////////////COLOR FACE///////////////////////////
static QColor color_chunks[3][5] =
{
    {0xffff00, 0x00ff00, 0x00ffff, 0xff00ff, 0x0000ff },
    {0xff0000, 0x00007f, 0x00aa7f, 0x00aa00, 0xaa007f },
    {0xaa0000, 0xaaaa00, 0x8c8c8c, 0xbfbfbf, QRgb(0x000000) }
};

FaceColor::FaceColor(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void FaceColor::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);

    QPainter p(this);
    paint(p.device());
}

//////////////////////COLOR GRID//////////////////////////////
void ColorGrid::paint(QPaintDevice* pd)
{
    QPainter p(pd);

    int dx = 6, dy = 6;
    for (int i = 1; i <= 3; i++) {
        dx = 6;

        for (int j = 1; j <= 5; j++) {
            p.save();
            p.fillRect(dx,dy, 22, 22, color_chunks[i-1][j-1]);

            QPen pen;
            if (_selM == i && _selN == j) {
                pen.setWidth(2);
                pen.setColor(0xe2d10e);
            } else
                pen.setColor(color_chunks[i - 1][j - 1].darker());
            p.setPen(pen);
            p.drawRect(dx, dy, 22, 22);
            dx += 29;
            p.restore();
        }
        dy += 29;
    }

    p.save();

    auto rt = QRect(6, dy + 4, width() - 12, 26);
    p.fillRect(rt, Qt::gray);
    
    if (rt.contains(_mousePos)) {
        QPen pen;
        pen.setColor(Qt::gray);
        pen.setWidth(2);
        p.drawRect(rt);
    }
    p.drawText(40, dy + 22, tr("TRANSPARENT"));
    
    p.restore();
}

void ColorGrid::mouseMoveEvent(QMouseEvent* e)
{
    auto pos = e->pos();
    int m = (pos.y() - 6) / 29;
    int n = (pos.x() - 6) / 29;
    m++; n++;

    if (m >= 1 && m <= 3 && n >= 1 && n <= 5) {
        int tly = (m - 1) * 22 + 7 * m;
        int bry = m * 22 + 7 * m;

        int tlx = (n - 1) * 22 + 7 * n;
        int brx = n * 22 + 7 * n;

        if (QRect(QPoint(tlx, tly), QPoint(brx, bry)).contains(pos)) {
            _selM = m;
            _selN = n;
        }
        update();
    }
    else
        _selM = _selN = -1;

    _mousePos = pos;

    FaceColor::mouseMoveEvent(e);
}

void ColorGrid::mousePressEvent(QMouseEvent* e)
{
    if (_selM != -1 && _selN != -1)
        emit colorChanged(color_chunks[_selM - 1][_selN - 1]);

    auto rt = QRect(6, 97, width() - 12, 26);
    if (rt.contains(e->pos()))
        emit colorChanged(Qt::transparent);

    FaceColor::mousePressEvent(e);
}

////////////COLOR STRIP//////////////
QSize ColorGrid::sizeHint() const
{
    return QSize(150, 130);
}

void ColorStrip::paint(QPaintDevice* pd)
{
}

void ColorStrip::mouseMoveEvent(QMouseEvent* e)
{
}

void ColorStrip::mousePressEvent(QMouseEvent* e)
{
}

QSize ColorStrip::sizeHint() const
{
    return QSize();
}
