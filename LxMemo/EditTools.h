#pragma once

#include <QWidget>
#include <QTextEdit>
#include "ui_EditTools.h"

class EditTools : public QWidget
{
    Q_OBJECT

public:
    EditTools(QTextEdit* doc, QWidget* parent = nullptr);
    ~EditTools();

    void UpdateTool(const QTextCharFormat& format);
signals:
    void changeBold(bool);
    void changeItalic(bool);
    void changeUnderline(bool);
    void changeStrikeout(bool);
    void changeFont(const QFont&);
    void changeFontSize(const QString&);
    void changeFontColor(const QColor&);
    void changeBackgroundColor(const QColor&);
    void changeFontUp(bool);
    void changeFontDown(bool);
private slots:
    void onFontColorChanged();
    void onFontBkColorChanged();
    void onFontChanged(const QFont& font);
protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void hideEvent(QHideEvent* event) override;
private:
    Ui::EditToolsClass ui;

    QTextEdit* _edit;
};
