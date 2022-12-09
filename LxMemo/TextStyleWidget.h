#pragma once

#include <QWidget>
#include "ui_TextStyleWidget.h"

class Graph;
class TextStyleWidget : public QWidget
{
    Q_OBJECT

public:
    TextStyleWidget(QWidget *parent = nullptr);
    ~TextStyleWidget();

    void SetGraph(Graph* graph);
signals:
    void visibleChanged(bool);
protected:
    virtual void paintEvent(QPaintEvent* e) override;
private slots:
    void onColorChanged(const QColor&);
    void onBkColorChanged(const QColor&);
    void onFontChanged(const QFont&);
    void onFontSizeChanged(const QString&);
private:
    void init();
private:
    Ui::TextStyleWidgetClass ui;

    Graph* graph_{ nullptr };
};
