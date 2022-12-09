#ifndef DOCKWINDOW_H
#define DOCKWINDOW_H

#include <QWidget>
#include <QVector>
#include "ui_dockwindow.h"

#include <QPropertyAnimation>

enum class StickArea
{
    kTop   ,
    kBottom,
    kLeft,
    kRight,
    //左右两侧
};

class DockWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DockWindow(QWidget *parent = nullptr);
    ~DockWindow();

    void SetViewport(const QRect& viewport);

    void SetStickArea(StickArea sa);
    void SetFixedStick(bool fixed);
    void SetPosition(const QPoint& pos);

    void SetSize(const QSize& size);
    void AddWidget(QWidget* w);

    void SetOpacity(int opacity);
    void SetAutoShrink(bool shrink);
    void SetAutoHeight(bool auto_height);
    void SetAutoWidth(bool auto_width);

    void Update();
protected:
    bool event(QEvent* event) override;
private slots:
    void on_single_column_clicked();

    void on_double_column_clicked();

private:
    void paint(QPaintEvent* e);
    void expand();
    void shrink();
    bool mouseInTitle();

    void addWidget(QWidget* w);
    void reLayout(int columns);
    void reNewLayout();

    bool atRight();
    bool atLeft();
    //顶fixed为真时，重新设置位置
    bool adjustPosition();
private:
    Ui::DockWidget ui;

    QRect viewport_{};

    std::atomic_bool is_shrinked_{false};
    QSize size_{};

    bool enable_shrink_{true};
    bool auto_height_{ false };
    bool auto_width_{ false };
//    QPropertyAnimation* expand_ani_{};
//    QPropertyAnimation* shrink_ani_{};

    QPropertyAnimation* expand_geo_{};
    QPropertyAnimation* shrink_geo_{};

    bool mouse_press_titlebar_{false};
    int x_diff_{0};
    int y_diff_{0};

    QVector<QWidget*> widgets_;
    int layout_type_{0};

    //是否固定，当dock位于左/右侧时，默认不固定，
    //当其位于上/下位置时，将会默认设置为固定位置
    bool is_fixed_{false};
    StickArea stick_area_{StickArea::kLeft};

    int opacity_{255};
};

#endif // DOCKWINDOW_H
