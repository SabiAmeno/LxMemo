#pragma once

#include <QToolBar>
#include <QLabel>
#include <QHBoxLayout>
//#include "ui_TitleBar.h"

class TitleBar : public QToolBar
{
    Q_OBJECT
public:
    TitleBar(QWidget* parent = nullptr);

    void SetTitle(const QString& title);
    void SetIcon(const QString& icon);
signals:
    void closeButtonClicked();
    void minimumButtonClicked();
    void maximumButtonClicked();

    void positionChanged(const QPoint&);
protected:
    virtual bool eventFilter(QObject* w, QEvent* e) override;
private:
    void initStyle();
private:
    //Ui::TitleBarClass ui;

    QLabel* icon_label_{ nullptr };
    QLabel* title_label_{ nullptr };

    QPoint pStart;
    bool   isMove{ false };
};
