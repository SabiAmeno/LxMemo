#include "FolderCreator.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QKeyEvent>

FolderCreator::FolderCreator(const QString& default_text, QWidget* parent)
    : NoFrameWidget(parent)
{
    ui.setupUi(this);

    setContentsMargins(6, 6, 6, 6);
    ui.lineEdit->setText(default_text);

    auto style = R"(
    .NoFrameWidget{
        border: 1px solid gray;
        border-radius: 8px;
    }
    QToolButton:pressed,
                 QToolButton:hover {
                     /*background-color:white;*/
                     border:0px groove white;
                 }
    QToolButton {
        border:0px groove gray;
        border-radius: 4px;
        background-color: rgba(0, 170, 0, 180);
        }
    QLineEdit {
        border-bottom: 1px solid gray;
        border-radius: 0px;
        background: #ffffff;
        height: 22px;
        selection-background-color: darkgray;
    }
    )";

    setStyleSheet(style);

    connect(ui.button_OK, &QToolButton::clicked, this, &FolderCreator::onConfirm);
    connect(ui.button_cancel, &QToolButton::clicked, this, &FolderCreator::onCancel);
}

FolderCreator::~FolderCreator()
{}

QString FolderCreator::getText()
{
    return ui.lineEdit->text();
}

void FolderCreator::onConfirm()
{
    emit windowClosed();
    close();
}

void FolderCreator::onCancel()
{
    close();
}

void FolderCreator::hideEvent(QHideEvent* e)
{
    //emit windowClosed();

    NoFrameWidget::hideEvent(e);
}

void FolderCreator::showEvent(QShowEvent* e)
{
    ui.lineEdit->setFocus();

    NoFrameWidget::showEvent(e);
}

void FolderCreator::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        emit ui.button_OK->clicked();

    NoFrameWidget::keyPressEvent(e);
}