#include "LxMemo.h"
#include "ListItem.h"
#include "TxtDelegate.h"
#include "MemoDialog.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QScroller>
#include <QFileDialog>
#include <QDateTime>
#include <QMenu>
#include <QInputDialog>
#include <queue>
#include "MainFrame.h"
#include "ItemMemo.h"
#include "ItemFolder.h"
#include "db.h"

LxMemo::LxMemo(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowIcon(QIcon(":/LxMemo/icons/memo.png"));
    setStyleSheet("background-color:transparent;");

    connect(ui.listWidget, &MainFrame::doubleClicked, this, &LxMemo::onEditMemo);
    connect(ui.listWidget, &MainFrame::customContextMenuRequested, this, &LxMemo::onDeleteMemo);
    //connect(ui.listWidget, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onItemEdit(const QModelIndex&)));

    QScroller::grabGesture(ui.cycleWidget, QScroller::LeftMouseButtonGesture);

    //connect(ui.cycleWidget, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onRecycleItemEdit(const QModelIndex&)));
    connect(ui.cycleWidget, &MainFrame::customContextMenuRequested, this, &LxMemo::onRecycleItemEdit);

    initDB();
    //loadMemo();

    //ui.listWidget->installEventFilter(this);
    //ui.listWidget->viewport()->installEventFilter(this);
    ui.listWidget->setMouseTracking(true);
    ui.cycleWidget->setMouseTracking(true);

    _cc = new ColorChunk(this);
    _cc->hide();

    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setIcon(QIcon(":/LxMemo/icons/memo.png"));
    QMenu* menu = new QMenu();
    auto mui = menu->addAction(QIcon(":/LxMemo/icons/memo.png"), "Show Main UI");
    auto ext = menu->addAction(QIcon(":/LxMemo/icons/Exit.png"), "Exit");
    _trayIcon->setContextMenu(menu);
    _trayIcon->show();

    connect(mui, &QAction::triggered, this, &LxMemo::ReDisplay);
    connect(ext, &QAction::triggered, this,
        [this] {
            for (auto& it : _opendDlg)
                it->close();
            close();
        });

    std::function<void(QSystemTrayIcon::ActivationReason reason)>
        cb = [this](QSystemTrayIcon::ActivationReason reason)
    {
        switch (reason)
        {
        case QSystemTrayIcon::Unknown:
            break;
        case QSystemTrayIcon::Context:
            break;
        case QSystemTrayIcon::DoubleClick:
            emit ReDisplay();
            break;
        case QSystemTrayIcon::Trigger:
            break;
        case QSystemTrayIcon::MiddleClick:
            break;
        default:
            break;
        }
    };

    connect(_trayIcon, &QSystemTrayIcon::activated, this, cb);

    std::function<void(const QColor&)> colselCB = [this](const QColor& color)
    {
        if (_currItem) {
            auto memo = qvariant_cast<SharedMemo>(_currItem->data(0));
            memo->SetColor(color);
            _currItem->setData(0, QVariant::fromValue(memo));

            if (_opendDlg.contains(memo->Id())) {
                auto dlg = _opendDlg.value(memo->Id());
                dlg->Widget<MemoDialog*>()->SetMemo(memo);
                //dlg->setStyleSheet(tr("background-color: %1;").arg(color.name(QColor::HexRgb)));
                dlg->SetPureStyle(color.name(QColor::HexArgb));
                dlg->Widget<MemoDialog*>()->setStyleSheet(tr("background-color: %1;").arg(color.name(QColor::HexArgb)));

                //dlg->update();
            }

            auto bytes = memo->PropertyToStream();
            std::unique_ptr<char> data(new char[bytes.size() + 256]{ 0 });
            int r = sprintf(data.get(), "update Memo set PROP=\"%s\" where ID=%u;",
                bytes.data(),
                memo->Id());
            ExecSQL(db, data.get());
        }
    };
    connect(_cc, &ColorChunk::ColorSelected, this, colselCB);

    QDir cache(".");
    bool r = cache.mkdir(".cache");
    if (r) {
        cache.setCurrent(".cache");
        SetFileAttributes((LPCWSTR)cache.absolutePath().unicode(), FILE_ATTRIBUTE_HIDDEN);
    }
}

LxMemo::~LxMemo()
{
    sqlite3_close_v2(db);
}

void LxMemo::onAddMemo()
{
    //Memo memo;
    addMemo(SharedMemo(new Memo()));
}

void LxMemo::addMemo(SharedMemo memo)
{
    ItemMemo* item = new ItemMemo();
    //item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    item->setData(0, QVariant::fromValue(memo));
    item->setData(Qt::UserRole, memo->Id());
    //item->setMemo(memo);

    //ui.listWidget->insertItem(0, item);
    ui.listWidget->insertItem(0, item);
    auto time = QDateTime::currentDateTime().toString();
    auto path = ui.listWidget->currentPath();

    char buffer[512]{ 0 };
    sprintf(buffer,
        R"(insert into Memo (ID, PROP, IN_RECYCLE, TIME, PATH) values(%u, "%s", 0, "%s", "%s");)",
        memo->Id(), memo->PropertyToStream().data(), time.toLocal8Bit().data(), path.toLocal8Bit().data());

    ExecSQL(db, buffer);

    QDir dir(tr(".cache"));
    dir.mkdir(tr("%1").arg(QString::number(memo->Id())));
}

void LxMemo::editMemo(Item* item)
{
    QMenu menu;
    //menu.setFixedWidth(100);
    auto delAction = menu.addAction(QIcon(":/LxMemo/icons/recycle-fill.png"), tr("Delete"));
    auto showAction = menu.addAction(QIcon(":/LxMemo/icons/show.png"), tr("Show"));
    auto swapColor = menu.addAction(QIcon(":/LxMemo/icons/eglass-color.png"), tr("Recolor"));
    menu.addSeparator();
    auto exportAction = menu.addAction(QIcon(":/LxMemo/icons/export.png"), tr("Export"));

    auto memo = qvariant_cast<SharedMemo>(item->data(0));
    uint32_t id = memo->Id();
    if (_opendDlg.contains(id)) {
        showAction->setIcon(QIcon(":/LxMemo/icons/close_eyes.png"));
        //showAction->setEnabled(false);
    }
    std::function<void(bool)> delCB = [this, item, id](bool)
    {
        //auto row = ui.listWidget->row(item);
        //ui.listWidget->takeItem(row);
        ui.listWidget->removeItem(item);
        if (_opendDlg.contains(id))
            _opendDlg.value(id)->close();

        char buffer[128]{ 0 };
        sprintf(buffer, "delete from Memo where ID=%u;", id);
        ExecSQL(db, buffer);
    };

    std::function<void(bool)> showCB = [this, item, memo](bool)
    {
        auto id = memo->Id();
        if (!_opendDlg.contains(id))
            onEditMemo(item);
        else
        {
            auto dlg = _opendDlg.value(id);
            dlg->close();
            //delete dlg;
            _opendDlg.remove(id);
        }
    };

    std::function<void(bool)> colorCB = [this, item](bool)
    {
        _cc->move(0, ui.listWidget->height());
        _cc->resize(width(), _cc->height());
        _cc->show();
    };

    std::function<void(bool)> exportHtml = [this, memo](bool)
    {
        auto htmlFile = QFileDialog::getSaveFileName(nullptr, tr("Save"), QString(), tr("*.html"));
        if (htmlFile.isEmpty())
            return;

        QFile file(htmlFile);
        file.open(QIODevice::WriteOnly);
        if (file.isOpen()) {
            auto html = memo->GetHtml();
            file.write(html.toUtf8());
            file.close();
        }
    };

    connect(delAction, &QAction::triggered, this, delCB);
    connect(showAction, &QAction::triggered, this, showCB);
    connect(swapColor, &QAction::triggered, this, colorCB);
    connect(exportAction, &QAction::triggered, this, exportHtml);

    auto p = ui.listWidget->cursor().pos();
    p = mapFromGlobal(p);
    menu.exec(mapToGlobal(p - QPoint(50, 0)));
}

void LxMemo::editFolder(Item* item)
{
    QMenu menu;
    //menu.setFixedWidth(100);
    auto delAction = menu.addAction(QIcon(":/LxMemo/icons/recycle-fill.png"), tr("Delete"));

    std::function<void(uint32_t)> delfromdb = [this](uint32_t id)
    {
        char buffer[128]{ 0 };
        sprintf(buffer, "delete from Memo where ID=%u;", id);
        ExecSQL(db, buffer);
    };

    std::function<void(bool)> delCB = [this, item, delfromdb](bool)
    {
        std::queue<Item*> itms;
        itms.push(item);

        while (!itms.empty()) {
            auto i = itms.front();
            itms.pop();
            if (!i->children().isEmpty()) {
                for (auto si : i->children())
                    itms.push(si);
            }

            if (IType::Type_File == i->type()) {
                auto id = i->data(Qt::UserRole).toUInt();
                delfromdb(id);
            }
        }
        item->removeRecursivly();
        if (item->parent())
            item->parent()->remove(item);
    };

    connect(delAction, &QAction::triggered, this, delCB);

    auto p = ui.listWidget->cursor().pos();
    p = mapFromGlobal(p);
    menu.exec(mapToGlobal(p - QPoint(50, 0)));
}

void LxMemo::editCycleMemo(Item* item)
{
    QMenu menu;
    //menu.setFixedWidth(100);
    auto rcvAction = menu.addAction(QIcon(":/LxMemo/icons/recovery.png"), tr("Rrecover"));
    auto delAction = menu.addAction(QIcon(":/LxMemo/icons/close.png"), tr("Delete"));

    auto memo = qvariant_cast<SharedMemo>(item->data(0));
    uint32_t id = memo->Id();

    std::function<void(bool)> delCB = [this, item, id](bool)
    {
        char buffer[512]{ 0 };
        sprintf(buffer, "delete from Memo where ID=%u;", id);
        ExecSQL(db, buffer);
        ui.cycleWidget->removeItem(item);

        ///删除该Memo同时删除所有的图片资源
        QDir dir(tr(".cache/%1").arg(QString::number(id)));
        if (dir.exists()) {
            dir.removeRecursively();
        }
    };

    std::function<void(bool)> rcvCB = [this, item, id](bool)
    {
        char buffer[128]{ 0 };
        sprintf(buffer, "update Memo set IN_RECYCLE=0 where ID=%u", id);

        ExecSQL(db, buffer);
        ui.cycleWidget->removeItem(item);
    };

    connect(delAction, &QAction::triggered, this, delCB);
    connect(rcvAction, &QAction::triggered, this, rcvCB);

    auto p = ui.cycleWidget->cursor().pos();
    p = mapFromGlobal(p);
    menu.exec(mapToGlobal(p - QPoint(50, 0)));
}

void LxMemo::editCycleFolder(Item* item)
{
}

void LxMemo::editItem(Item* item)
{
    //if (!index.isValid())
    //    return;
    //auto item = ui.listWidget->item(index.row());
    _currItem = item;

    if (IType::Type_File == item->type())
        editMemo(item);
    else
        editFolder(item);
}

void LxMemo::editRecycleItem(Item* item)
{
    switch (item->type())
    {
    case IType::Type_File:
    {
        editCycleMemo(item);
        break;
    }
    case IType::Type_Folder:
    {
        editCycleFolder(item);
        break;
    }
    default:
        break;
    }
}

void LxMemo::onExchangePanel()
{
    int ind = ui.stackedWidget->currentIndex();
    ind++;
    ind %= 2;
    ui.stackedWidget->SetCurrentIndex(ind);

    auto button = qobject_cast<QToolButton*>(sender());
    if (button) {
        if (ind == 1) {
            button->setIcon(QIcon(":/LxMemo/icons/symbol.png"));
            button->setToolTip(tr("Memos"));
        }
        else {
            button->setIcon(QIcon(":/LxMemo/icons/recycle-fill.png"));
            button->setToolTip(tr("Recycle"));
        }
    }

    //recycle
    //FIXME: 可能效率会比较低
    if (ind == 1) {
        ui.cycleWidget->clear();
        initRecycleMemo();
    }
    else {
        ui.listWidget->clear();
        initDB();
    }
}

void LxMemo::onRecycleItemEdit(const QPoint& pos)
{
    auto item = ui.cycleWidget->item(pos);
    //auto p = ui.cycleWidget->cursor().pos();
    //auto rect = ui.cycleWidget->visualItemRect(item);
    //p = mapFromGlobal(p);

    //_cc->hide();

    //auto tR = rect.topRight();
    //auto r = QRect(tR - QPoint(25, -50), tR - QPoint(5, -60));
    //if (r.contains(p)) {
    editRecycleItem(item);
    //}
}

void LxMemo::onDeleteMemo(const QPoint& pos)
{
    auto item = ui.listWidget->item(pos);
    if (item)
        editItem(item);
    else {
        QMenu menu;
        auto addAct = menu.addAction(QIcon(":/LxMemo/icons/add.png"), "New");
        auto importAction = menu.addAction(QIcon(":/LxMemo/icons/import.png"), tr("Import"));
        auto addFolder = menu.addAction(QIcon(""), tr("Add Folder"));

        std::function<void(bool)> importHtml = [this](bool)
        {
            auto htmlFile = QFileDialog::getOpenFileName(nullptr, tr("Open"), QString(), "*.html;*.txt");
            if (htmlFile.isEmpty())
                return;

            QFile f(htmlFile);
            f.open(QIODevice::ReadOnly);
            if (f.isOpen()) {
                auto content = f.readAll();
                auto memo = SharedMemo(new Memo());
                memo->SetHtml(content);
                addMemo(memo);
                f.close();
            }
        };

        std::function<void(bool)> addfolder = [this](bool)
        {
            auto name = QInputDialog::getText(nullptr, tr("Folder Name"), "Input");
            if (!name.isEmpty()) {
                auto item = new ItemFolder();
                item->setData(1, name);
                ui.listWidget->addItem(item);
            }
        };

        connect(addAct, &QAction::triggered, this, &LxMemo::onAddMemo);
        connect(importAction, &QAction::triggered, this, importHtml);
        connect(addFolder, &QAction::triggered, this, addfolder);

        menu.exec(mapToGlobal(pos));
    }
}

void LxMemo::onItemEdit(Item* item)
{
    //auto item = ui.listWidget->item(index.row());
    //auto p = ui.listWidget->cursor().pos();
    //auto rect = ui.listWidget->visualItemRect(item);
    //p = mapFromGlobal(p);

    _cc->hide();

    //auto tR = rect.topRight();
    //auto r = QRect(tR - QPoint(25, -50), tR - QPoint(5, -60));
    //if (r.contains(p)) {
    editItem(item);
    //}
}

int onUpdateData(void* w, int rows, char** rowData, char** colNames)
{
    auto item = (Item*)w;

    //uint32_t id = std::stoull(rowData[0]);
    QString html = QString::fromLocal8Bit(QByteArray::fromBase64(rowData[0]));
    //QString fontData = rowData[2];
    //Memo memo(id);
    //memo.SetHtml(html);
    auto memo = qvariant_cast<SharedMemo>(item->data(0));
    memo->SetHtml(html);
    ///FIXME: 设置为0，为了是使界面能够重新计算每个Item的rect
    ///如果有其它方法，需要将这里的代码删除
    /// note：可能会产生bug
    item->setData(0, 0);
    item->setData(0, QVariant::fromValue(memo));

    return 0;
}

void LxMemo::onUpdateMemo(uint32_t id)
{
    Item* item = nullptr;
    for (int i = 0; i < ui.listWidget->itemCount(); i++)
        if (ui.listWidget->item(i)->data(Qt::UserRole).toUInt() == id) {
            item = ui.listWidget->item(i);
            break;
        }
    if (item) {
        char buffer[128]{ 0 };
        sprintf(buffer, "select DATA from Memo where ID=%u;", id);

        ExecSQL(db, buffer, onUpdateData, item);
    }

    ui.listWidget->update();
}

void LxMemo::onMemoClosed()
{
    auto obj = qobject_cast<LucencyDialog*>(sender());
    auto mid = obj->Widget<MemoDialog*>()->MemoId();
    _opendDlg.remove(mid);

    ///FIXME: 若不重新显示主界面的话，待所有的
    ///memo退出后，系统的TrayIcon也会退出，如果找到
    ///产生该bug的原因，可以去掉这个代码
    if (_opendDlg.isEmpty())
        emit ReDisplay();
}

void LxMemo::loadMemo(const QString& path, SharedMemo memo)
{
    ItemMemo* item = new ItemMemo();
    //item->setMemo(memo);
    item->setData(0, QVariant::fromValue(memo));
    item->setData(Qt::UserRole, memo->Id());

    auto pitem = ui.listWidget->getItemByPath(path);
    pitem->add(item);
}

void LxMemo::loadRecyleMemo(const QString& path, SharedMemo memo)
{
    ItemMemo* item = new ItemMemo();
    //item->setMemo(memo);
    item->setData(0, QVariant::fromValue(memo));
    item->setData(Qt::UserRole, memo->Id());

    auto pitem = ui.cycleWidget->getItemByPath(path);
    //ui.cycleWidget->insertItem(0, item);
    pitem->add(item);
}

int onReadDataFromDB(void* lx, int rows, char** rowData, char** colNames)
{
    auto lxmemo = (LxMemo*)lx;

    uint32_t id = std::stoull(rowData[0]);
    QByteArray prop = rowData[1];
    QString snapshot = QString::fromLocal8Bit(QByteArray::fromBase64(rowData[2]));
    QString time = QString::fromLocal8Bit(rowData[3]);
    QString path = QString::fromLocal8Bit(rowData[4]);

    auto memo = SharedMemo(new Memo(id));
    memo->SetSnapshot(snapshot);
    memo->StreamToProperty(prop);
    memo->SetTime(time);

    lxmemo->loadMemo(path, memo);

    return 0;
}

void LxMemo::initDB()
{
    db = NewSqlite3Handle("memo.db");
    char* szErrMsg = 0;

    auto sql = R"(
        CREATE TABLE IF NOT EXISTS Memo(
            ID BIGINT PRIMARY KEY NOT NULL,
            DATA TEXT,
            PROP TEXT,
            IN_RECYCLE INTEGER,
            SNAPSHOT TEXT,
            TIME TEXT,
            PATH TEXT);
    )";
    ExecSQL(db, sql);

    //sql = "select ID, DATA, PROP from Memo where IN_RECYCLE=0;";
    sql = "select ID, PROP, SNAPSHOT, TIME, PATH from Memo where IN_RECYCLE=0;";
    ExecSQL(db, sql, onReadDataFromDB, this);
}

int onReadCycleMemo(void* lx, int rows, char** rowData, char** colNames)
{
    auto lxmemo = (LxMemo*)lx;

    uint32_t id = std::stoull(rowData[0]);
    QByteArray prop = rowData[1];
    QString snapshot = QString::fromLocal8Bit(QByteArray::fromBase64(rowData[2]));
    QString time = QString::fromLocal8Bit(rowData[3]);
    QString path = QString::fromLocal8Bit(rowData[4]);

    auto memo = SharedMemo(new Memo(id));
    memo->SetSnapshot(snapshot);
    memo->StreamToProperty(prop);
    memo->SetTime(time);

    lxmemo->loadRecyleMemo(path, memo);

    return 0;
}

void LxMemo::initRecycleMemo()
{
    const char* sql = "select ID, PROP, SNAPSHOT, TIME, PATH from Memo where IN_RECYCLE=1;";
    ExecSQL(db, sql, onReadCycleMemo, this);
}

void LxMemo::showMemo(SharedMemo memo)
{
    auto fontDlg = QSharedPointer<LucencyDialog>(new LucencyDialog());

    fontDlg->SetOpacity(0);
    fontDlg->SetPureStyle(memo->GetColor().name(QColor::HexArgb));
    fontDlg->SetIcon("", false);
    fontDlg->SetAttribute(~WA_MAXIMUM_WINDOW);

    MemoDialog* dlg = new MemoDialog(memo, db);
    auto title = QString::fromLocal8Bit("ID:%1").arg(memo->Id());
    dlg->setWindowTitle(title);
    dlg->SetMemo(memo);

    uint32_t id = memo->Id();
    _opendDlg.insert(memo->Id(), fontDlg);
    fontDlg->SetWidget(dlg);
    fontDlg->resize(400, 350);

    srand(time(nullptr));
    auto TR = rect().topRight();
    TR = mapToGlobal(TR);

    TR.rx() += rand() % 200;
    TR.ry() += rand() % 150;

    fontDlg->move(TR);

    fontDlg->show();

    connect(fontDlg.data(), &LucencyDialog::windowClosed, dlg, &MemoDialog::onWindowClosed);
    connect(fontDlg.data(), &LucencyDialog::windowClosed, this, &LxMemo::onMemoClosed);
    connect(dlg, &MemoDialog::MemoUpdate, this, &LxMemo::onUpdateMemo, Qt::QueuedConnection);

    anim::StartOpacityAnimation(fontDlg.get(), 0, 0.95);
}

struct MemoContext
{
    LxMemo* lx;
    SharedMemo memo;
};
int loadMemoData(void* lx, int rows, char** rowData, char** colNames)
{
    auto ctx = (MemoContext*)lx;

    QString html = QString::fromLocal8Bit(QByteArray::fromBase64(rowData[0]));
    ctx->memo->SetHtml(html);

    ctx->lx->showMemo(ctx->memo);

    return 0;
}

void LxMemo::onEditMemo(Item* item)
{
    auto memo = qvariant_cast<SharedMemo>(item->data(0));
    if (!memo)
        return;

    auto isShow = _opendDlg.contains(memo->Id());
    if (isShow)
        return;

    if (memo->GetHtml().isEmpty()) {
        auto sql = QString("select DATA from Memo where ID=%1").arg(QString::number(memo->Id()));

        MemoContext ctx = { this, memo };
        ExecSQL(db, sql.toUtf8().data(), loadMemoData, &ctx);
    }
    else {
        showMemo(memo);
    }
}

bool LxMemo::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui.listWidget) {
        if (event->type() == QEvent::Resize) {
            if (!_cc->isHidden()) {
                QResizeEvent* ev = (QResizeEvent*)event;
                _cc->move(0, ui.listWidget->height());
                _cc->resize(width(), _cc->height());
            }
        }
    }
    return QWidget::eventFilter(object, event);
}

namespace anim
{
    template<typename T>
    void AnimationImpl(QWidget* target, const T& from, const T& to, QEasingCurve easing, int duration, const QByteArray& prop)
    {
        QPropertyAnimation* ani = new QPropertyAnimation(target, prop);
        ani->setStartValue(from);
        ani->setEndValue(to);
        ani->setEasingCurve(easing);
        ani->setDuration(duration);
        ani->start(QAbstractAnimation::DeleteWhenStopped);
    }
    //////////////////////ANIMATION FUNCTION//////////////////////////////
    void StartOpacityAnimation(QWidget* target, double from, double to, QEasingCurve easing, int duration)
    {
        AnimationImpl(target, from, to, easing, duration, "windowOpacity");
    }
    void StartGeometryAnimation(QWidget* target, const QRect& from, const QRect& to, QEasingCurve easing, int duration)
    {
        AnimationImpl(target, from, to, easing, duration, "geometry");
    }
}