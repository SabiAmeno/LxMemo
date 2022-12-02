#include "MessageBox.h"

MessageDialog::MessageDialog(QWidget* parent)
    : NoFrameWidget(parent)
{
    ui.setupUi(this);

    auto style = R"(  QToolButton:pressed,
                 QToolButton:hover {
                     background-color:white;
                     border:0px groove white;
                 }
                QToolButton{border-radius: 4px; height: 16px;}
                QToolButton#toolButton {background-color: rgba(0, 170, 0, 180);}
                QToolButton#toolButton_2 {background-color: rgba(208, 0, 0, 200);}
                )";
    ui.toolButton->setStyleSheet(style);
    ui.toolButton_2->setStyleSheet(style);
}

MessageDialog::~MessageDialog()
{}

void MessageDialog::setText(const QString& text)
{
    ui.label_2->setText(text);
}

void MessageDialog::on_toolButton_clicked()
{
    emit accept();
    close();
}

void MessageDialog::on_toolButton_2_clicked()
{
    emit reject();
    close();
}