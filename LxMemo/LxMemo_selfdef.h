#pragma once

#include <QWidget>
#include <QSystemTrayIcon>
#include <QEasingCurve>

#include "ui_LxMemo.h"
#include "lucencydialog.h"
#include "Memo.h"
#include "ColorChunk.h"
#include "Item.h"

#include "sqlite3.h"

class LxMemo : public QWidget
{
    Q_OBJECT

public:
    LxMemo(QWidget* parent = nullptr);
    ~LxMemo();

    void loadMemo(const QString& path, SharedMemo memo);
    void loadRecyleMemo(const QString& path, SharedMemo memo);
    void showMemo(SharedMemo memo);

    sqlite3* db = nullptr;

signals:
    void ReDisplay();
public slots:
    void onAddMemo();
    void onExchangePanel();
private slots:
    void onEditMemo(Item* item);
    void onRecycleItemEdit(const QPoint& pos);
    void onDeleteMemo(const QPoint& pos);
    void onItemEdit(Item* item);
    void onUpdateMemo(uint32_t id);
    void onMemoClosed();

protected:
    virtual bool eventFilter(QObject* object, QEvent* event) override;
private:
    void initDB();
    void initRecycleMemo();
    void editItem(Item* item);
    void editRecycleItem(Item* item);
    void addMemo(SharedMemo memo);

    void editMemo(Item* item);
    void editFolder(Item* item);

    void editCycleMemo(Item* item);
    void editCycleFolder(Item* item);
private:
    Ui::LxMemoClass ui;

    //QGraphicsOpacityEffect* _effect;
    //QPropertyAnimation* _opacityAni;
    QSystemTrayIcon* _trayIcon;

    QMap<uint32_t, QSharedPointer<LucencyDialog>> _opendDlg;
    //LucencyDialog* fontDlg;
    ColorChunk* _cc;
    Item* _currItem{ nullptr };

    bool _startSlide{ false };
    QPoint _slidePos;
};

namespace anim
{
    void StartOpacityAnimation(
        QWidget* target,
        double from,
        double to,
        QEasingCurve easing = QEasingCurve::Linear,
        int duration = 200);

    void StartGeometryAnimation(
        QWidget* target,
        const QRect& from,
        const QRect& to,
        QEasingCurve easing = QEasingCurve::Linear,
        int duration = 200);
}
