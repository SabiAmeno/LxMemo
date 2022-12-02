#pragma once

#include <QDialog>
#include "NoFrameWidget.h"
#include "ui_MessageBox.h"

class MessageDialog : public NoFrameWidget
{
    Q_OBJECT

public:
    MessageDialog(QWidget* parent = nullptr);
    ~MessageDialog();

    void setText(const QString& text);

signals:
    void accept();
    void reject();
private slots:
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
private:
    Ui::MessageBoxClass ui;
};
