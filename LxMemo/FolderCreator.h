#pragma once

#include <QDialog>
#include "ui_FolderCreator.h"
#include "NoFrameWidget.h"

class FolderCreator : public NoFrameWidget
{
    Q_OBJECT

public:
    FolderCreator(const QString& default_text, QWidget* parent = nullptr);
    ~FolderCreator();

    QString getText();

signals:
    void windowClosed();
private slots:
    void onConfirm();
    void onCancel();
protected:
    virtual void hideEvent(QHideEvent* e) override;
    virtual void showEvent(QShowEvent* e) override;
    virtual void keyPressEvent(QKeyEvent* e) override;
private:
    Ui::FolderCreatorClass ui;
};
