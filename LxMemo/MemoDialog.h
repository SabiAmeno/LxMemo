#pragma once

#include <QDialog>
#include <QTimer>
#include <QTextList>
#include "ui_MemoDialog.h"
#include "Memo.h"
//#include "sqlite3.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "Searcher.h"
#include "EditTools.h"
#include "lucencydialog.h"
//#include "ColorChunk.h"
#include "SizeSelector.h"
#include "db.h"
#include "NoFrameWidget.h"

#include <condition_variable>

class MemoDialog : public NoFrameWidget
{
    Q_OBJECT

public:
    MemoDialog(SharedMemo memo, DB* db);
    ~MemoDialog();

    void SetMemo(SharedMemo memo);
    uint32_t MemoId();

    void SetTitle(const QString& title);
signals:
    void MemoUpdate(MemoDialog*);
    void windowClosed();
public slots:
    void onWindowClosed();
protected:
    virtual void closeEvent(QCloseEvent* e) override;
    virtual void keyPressEvent(QKeyEvent* e) override;
    virtual bool eventFilter(QObject* watch, QEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
private slots:
    void onTextBold(bool checked);
    void onTextItalic(bool checked);
    void onTextUnderline(bool checked);
    void onTextStrikeout(bool checked);
    void onTextFontSize(const QString& s);
    void onInsertImage();
    void onSymbolChecked(bool checked = false);
    void onMenuPop(const QPoint& pos);
    void onSelectChanged();
    void onTextSearch(bool check);
    void onImageSelected(const QTextImageFormat& format);

    void onTextFontChanged(const QFont& font);
    void onTextColorChanged(const QColor& color);
    void onTextBkColorChanged(const QColor& color);

    void onTextColorUp(bool);
    void onTextColorDown(bool);
private:
    void init();
    void changeProp(auto cb);
    void save();
    void saveInThread();
private:
    Ui::MemoDialogClass ui;
    DB* db_;
    SharedMemo memo_;

    QGraphicsOpacityEffect* effect_;
    //QPropertyAnimation* opacity_animation_;
    //QTextList* text_list_;

    Searcher* searcher_;
    EditTools* edit_tool_;
    //LucencyDialog* edit_dialog_;
    LucencyDialog* size_selector_dialog_;
    SizeSelector* size_selector_;

    std::atomic_bool keep_ = true;
    std::atomic_bool save_right_ = false;
    std::thread* thread_ = nullptr;
    std::mutex mtx_;
    std::condition_variable cv_;

    bool text_selected_ = false;
};
