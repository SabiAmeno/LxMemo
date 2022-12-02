#pragma once

#include <QWidget>
#include <QTextFormat>
#include "ui_SizeSelector.h"
#include "lucencydialog.h"

class SizeSelector : public QWidget
{
    Q_OBJECT

public:
    SizeSelector(QWidget* parent = nullptr);
    ~SizeSelector();

    void Init(const QString& img);

signals:
    void SelectFinished(const QTextImageFormat&);

private slots:
    void on_toolButton_clicked();
private:
    Ui::SizeSelectorClass ui;

    QString _imgFile;
    QImage _img;
};
