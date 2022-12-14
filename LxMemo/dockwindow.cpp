#include "dockwindow.h"
#include <QMoveEvent>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QTimer>
#include <QDebug>

const static int kDockWidth = 8;

DockWindow::DockWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    reNewLayout();

    setWindowFlags(/*Qt::CoverWindow | Qt::Window |*//* Qt::FramelessWindowHint |*/ Qt::SubWindow);
    //setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    setAutoFillBackground(true);
    setAttribute(Qt::WA_TranslucentBackground);

#if 1 ///此处使用阴影边框，会影响窗口stylesheet的动态设置
    //设置阴影边框
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect();
    shadow_effect->setOffset(0, 0);

    //阴影颜色
    shadow_effect->setColor(QColor(38, 78, 119, 100));

    //阴影半径
    shadow_effect->setBlurRadius(16);

    setGraphicsEffect(shadow_effect);
#endif

    expand_geo_ = new QPropertyAnimation(this, "geometry");
    shrink_geo_ = new QPropertyAnimation(this, "geometry");
    expand_geo_->setDuration(200);
    shrink_geo_->setDuration(200);


    connect(expand_geo_, &QPropertyAnimation::valueChanged, this,
            [this](const QVariant& v) {
                auto size = v.toRect();
                if(size.height() >= kDockWidth + 5 || size.width() >= kDockWidth + 5) {
                    ui.frame->show();
                    if(!is_fixed_)
                        ui.title_frame->show();
                }
    });

    connect(shrink_geo_, &QPropertyAnimation::valueChanged, this,
            [this](const QVariant& v) {
                auto size = v.toRect();
                if(size.height() < kDockWidth + 5 || size.width() < kDockWidth + 5) {
                    ui.frame->hide();
                    if(!is_fixed_)
                        ui.title_frame->hide();
                }
    });

    connect(expand_geo_, &QPropertyAnimation::finished, this, [this]{is_shrinked_ = false;});
    connect(shrink_geo_, &QPropertyAnimation::finished, this, [this]{is_shrinked_ = true;});
}

DockWindow::~DockWindow()
{
    delete expand_geo_;
    delete shrink_geo_;
}

void DockWindow::SetViewport(const QRect& viewport)
{
    viewport_ = viewport;

    if (auto_height_) {
        size_.setHeight(viewport_.height());
    }
    if (auto_width_) size_.setWidth(viewport_.width());

    resize(size_);

    Update();
}

void DockWindow::SetStickArea(StickArea sa)
{
    stick_area_ = sa;

    resize(size_);

    shrink();
}

void DockWindow::SetFixedStick(bool fixed)
{
    is_fixed_ = fixed;
    if (is_fixed_) {
        ui.title_frame->hide();
        //adjustPosition();
    }
    else {
        SetPosition(QPoint(viewport_.left(), (viewport_.height() - size_.height()) / 2));
    }
}

void DockWindow::SetPosition(const QPoint &pos)
{
    move(pos);
}

void DockWindow::SetSize(const QSize &size)
{
    size_ = size;
}

void DockWindow::AddWidget(QWidget *w)
{
    widgets_.append(w);
    addWidget(w);
}

void DockWindow::SetOpacity(int opacity)
{
    opacity_ = opacity;

    update();
}

void DockWindow::SetAutoShrink(bool shrink)
{
    enable_shrink_ = shrink;
}

void DockWindow::SetAutoWidth(bool auto_width)
{
    auto_width_ = auto_width;
}

void DockWindow::SetAutoHeight(bool auto_height)
{
    auto_height_ = auto_height;
}

void DockWindow::Update()
{
    //if(StickArea::kBottom == stick_area_) {
    //    int x = (viewport_.width() - size_.width())/2;
    //    int y = viewport_.height() - size_.height() - 12;
    //    if(is_shrinked_)
    //        y = viewport_.bottom() - kDockWidth;
    //    SetPosition(QPoint(x, y));
    //}

    if (!is_shrinked_ && is_fixed_) {
        switch (stick_area_)
        {
        case StickArea::kTop:
            break;
        case StickArea::kBottom:
            break;
        case StickArea::kLeft:
        {
            int x = viewport_.left();
            int y = viewport_.top() + (viewport_.height() - size_.height()) / 2;
            SetPosition(QPoint(x, y));
            break;
        }
        case StickArea::kRight:
        {
            int x = viewport_.right() - size_.width();
            int y = viewport_.top() + (viewport_.height() - size_.height()) / 2;
            SetPosition(QPoint(x, y));
            break;
        }
        default:
            break;
        }
    }
    update();
}

bool DockWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter:
    {
        ///延时300ms考虑到鼠标滑动的速度过快而出现的闪烁问题
        QTimer::singleShot(300,
            [this]{
                bool in = rect().contains(mapFromGlobal(cursor().pos()));
                if(in && is_shrinked_)
                    expand();
            }
        );
        break;
    }
    case QEvent::Leave:
    {
        QTimer::singleShot(300,
            [this]{
            if(!is_shrinked_) {
                if(StickArea::kLeft == stick_area_ 
                    || StickArea::kRight == stick_area_) {
                    if(atLeft() || atRight())
                        shrink();
                }
                else {
                    shrink();
                }
                }
            }
        );

        break;
    }
    case QEvent::Paint:
    {
        auto e = (QPaintEvent*)event;
        paint(e);
        event->accept();
        break;
    }
    case QEvent::MouseButtonPress:
    {
        auto e = (QMouseEvent*)event;
        if(mouseInTitle()) {
            mouse_press_titlebar_ = true;
            y_diff_ = e->pos().y();
            x_diff_ = e->pos().x();
        }
        break;
    }
    case QEvent::MouseMove:
    {
        ///是否已经按下鼠标
        if(mouse_press_titlebar_ && !is_fixed_) {
            auto e = (QMouseEvent*)event;

            if(is_shrinked_) {
                int diffy = e->pos().y() - y_diff_;
                int posy = pos().y() + diffy;
                if(posy < viewport_.height()-5 && posy > 0)
                    SetPosition(QPoint(pos().x(), posy));
            } else {
                int diffx = e->pos().x() - x_diff_;
                int diffy = e->pos().y() - y_diff_;
                int posx = pos().x() + diffx;
                int posy = pos().y() + diffy;
                if(viewport_.contains(QPoint(posx, posy)))
                    SetPosition(QPoint(posx, posy));
            }
        }
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        if (mouse_press_titlebar_ && !is_fixed_) {
            if (atLeft()) {
                if (!is_shrinked_ && layout_type_ != 1)
                    reLayout(1);
                SetPosition(QPoint(0, y()));
            }
            else if (atRight()) {
                if (!is_shrinked_ && layout_type_ != 1)
                    reLayout(1);
                SetPosition(QPoint(viewport_.width() - size_.width(), pos().y()));
            }
        }
        mouse_press_titlebar_ = false;

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

void DockWindow::paint(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing);

    auto pat = palette();
    auto color = pat.color(QPalette::Window);

    if (is_shrinked_) {
        color = color.darker();
        color.setAlpha(opacity_);
    }
    QPainterPath path;
    path.addRoundedRect(rect(), 1, 1);
    p.fillPath(path, color);
}

void DockWindow::expand()
{
    if(!mouseInTitle()) {
        auto start = QRect(geometry().topLeft(), QSize(size_.width(), kDockWidth));
        auto end = start;

        switch (stick_area_) {
        case StickArea::kTop:
            end = start.adjusted(0, 0, 0, size_.height());
            break;
        case StickArea::kBottom:
        {
            end = start.adjusted(0, -size_.height() + kDockWidth, 0, 0);
            break;
        }
        case StickArea::kLeft:
        {
            start.setSize(QSize(kDockWidth, size_.height()));

            if(atLeft()) {
                end = start.adjusted(0, 0, size_.width() - kDockWidth, 0);
            } else {
                end = start.adjusted(-size_.width() + kDockWidth, 0, 0, 0);
            }
            break;
        }
        case StickArea::kRight:
        {
            start.setSize(QSize(kDockWidth, size_.height()));
            break;
        }
        }

        expand_geo_->setStartValue(start);
        expand_geo_->setEndValue(end);
        expand_geo_->start();
    }
}

void DockWindow::shrink()
{
    if(!enable_shrink_)
        return ;

    if(!mouseInTitle()) {
        auto start = QRect(geometry().topLeft(), size_);
        auto end = start;

        switch (stick_area_) {
        case StickArea::kTop:
            end = start.adjusted(0, 0, 0, -size_.height() + kDockWidth);
            break;
        case StickArea::kBottom:
        {
            end = start.adjusted(0, size_.height() - kDockWidth, 0, 0);
            break;
        }
        case StickArea::kLeft:
        {
            if(atLeft())
                end = start.adjusted(0, 0, -size_.width() + kDockWidth, 0);
            else
                end = start.adjusted(size_.width() - kDockWidth, 0, 0, 0);
            break;
        }
        case StickArea::kRight:
        {
            //start.setSize(QSize(kDockWidth, size_.height()));
            break;
        }
        }

        shrink_geo_->setStartValue(start);
        shrink_geo_->setEndValue(end);
        shrink_geo_->start();
    }
}

bool DockWindow::mouseInTitle()
{
    auto title_area = ui.title_frame->rect();
    auto pos = cursor().pos();
    pos = mapFromGlobal(pos);

    return title_area.contains(pos);
}

void DockWindow::addWidget(QWidget *w)
{
    auto grid = (QGridLayout*)ui.frame->layout();
    int count = grid->count();

    switch (stick_area_) {
    case StickArea::kTop:
    case StickArea::kBottom:
    {
        grid->addWidget(w, 0, count);
        break;
    }
    default:
    {
        grid->addWidget(w, count, 0);
        break;
    }
    }
}

void DockWindow::reLayout(int columns)
{
    layout_type_ = columns;

    reNewLayout();
    auto grid = (QGridLayout*)ui.frame->layout();

    for(int i = 0;i < widgets_.size();i++)
        grid->addWidget(widgets_[i], i / columns, i % columns);

    ui.frame->setLayout(grid);

    int w = size_.width() * columns;
    int h = size_.height() / columns;

    resize(w, h);
}

void DockWindow::reNewLayout()
{
    auto old = ui.frame->layout();
    if(old) {
        while(old->count())
        {
            auto w = old->itemAt(0)->widget();
            w->setParent(nullptr);
            old->removeWidget(w);
        }
        delete old;
    }

    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(0,0,0,0);
    //grid->setSpacing(2);
    ui.frame->setContentsMargins(0, 0, 0, 0);
    ui.frame->setLayout(grid);
}

bool DockWindow::atRight()
{
    return geometry().right() + 5 >= viewport_.width();
}

bool DockWindow::atLeft()
{
    return pos().x() <= 5;
}

bool DockWindow::adjustPosition()
{
    int x = (viewport_.width() - size_.width()) / 2;
    int y = 0;
    if (StickArea::kBottom == stick_area_) {
        y = viewport_.height() - size_.height();
    }
    else if (StickArea::kLeft == stick_area_) {
        x = 0;
        y = (viewport_.height() - size_.height()) / 2;
    }
    else if (StickArea::kRight == stick_area_) {
        x = (viewport_.width() - size_.width());
    }

    SetPosition(QPoint(x, y));

    return true;
}

void DockWindow::on_single_column_clicked()
{
    reLayout(1);
}


void DockWindow::on_double_column_clicked()
{
    reLayout(2);
}

