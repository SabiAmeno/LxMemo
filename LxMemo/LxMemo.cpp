#include "LxMemo.h"
#include "ListItem.h"
//#include "TxtDelegate.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QScroller>
#include <QFileDialog>
#include <QDateTime>
#include <QInputDialog>
#include <QMenu>
#include <QMimeData>
#include <QClipboard>
#include "MessageBox.h"
#include "MessageShower.h"
#include "FolderCreator.h"
#include "Folder.h"
#include "util.h"
#include "db.h"
#include "NoFrameWidget.h"
#include "GraphNote.h"
#include "Canvas.h"

#include <Windows.h>

#define SEARCH_ENABLED false

LxMemo::LxMemo(QWidget* parent)
	: NoFrameWidget(parent)
{
	ui.setupUi(this);

	setWindowIcon(QIcon(":/LxMemo/icons/memo.png"));
	//setStyleSheet("background-color:transparent;");
	//setStyleSheet("background-color:#55000000;");
	ui.widget->SetTitle(tr("LxMemo"));
	ui.note_wall->SetBackground(Qt::black);

	ui.lineEdit->setVisible(SEARCH_ENABLED);
	ui.toolButton->setVisible(SEARCH_ENABLED);

	ui.button_forward->setVisible(false);
	ui.button_back->setEnabled(false);

	connect(ui.widget, &TitleBar::closeButtonClicked, this, [this] {close(); });
	connect(ui.widget, &TitleBar::minimumButtonClicked, this, [this] {showMinimized(); });
	connect(ui.widget, &TitleBar::positionChanged, this, [this](const QPoint& diff) {move(pos() + diff); });

	/* connect(ui.listWidget, &QListWidget::doubleClicked, this, &LxMemo::onEditMemo);
	 connect(ui.listWidget, &QListWidget::customContextMenuRequested, this, &LxMemo::onDeleteMemo);
	 connect(ui.listWidget, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onItemEdit(const QModelIndex&)));*/

	connect(ui.note_wall, &NoteWall::NoteClicked, this, &LxMemo::onItemEdit);
	connect(ui.note_wall, &NoteWall::NoteDoubleClicked, this, &LxMemo::onMemoDisplay);
	connect(ui.note_wall, &NoteWall::CustomClicked, this, &LxMemo::onMemoEdit);

	connect(ui.note_wall, &NoteWall::MemoAddTriggered, this, &LxMemo::onAddMemo);
	connect(ui.note_wall, &NoteWall::ImageAddTriggered, this, &LxMemo::onAddGraph);
	connect(ui.note_wall, &NoteWall::FolderAddTriggered, this, &LxMemo::onAddFolder);
	connect(ui.note_wall, &NoteWall::MemoImportTriggered, this, &LxMemo::onImportMemo);

	connect(ui.button_back, &QToolButton::clicked, this, &LxMemo::onBack);
	connect(ui.button_forward, &QToolButton::clicked, this, &LxMemo::onForward);
	connect(ui.button_new_folder, &QToolButton::clicked, this, &LxMemo::onAddFolder);

	initDB();

	saver_ = new MetaSaver(db_);

	color_chunk_ = new ColorChunk(this);
	color_chunk_->hide();

	/*tray_icon_ = new QSystemTrayIcon(this);
	tray_icon_->setIcon(QIcon(":/LxMemo/icons/memo.png"));
	QMenu* menu = new QMenu();
	auto mui = menu->addAction(QIcon(":/LxMemo/icons/memo.png"), "Show Main UI");
	auto ext = menu->addAction(QIcon(":/LxMemo/icons/Exit.png"), "Exit");
	tray_icon_->setContextMenu(menu);
	tray_icon_->show();

	connect(mui, &QAction::triggered, this, [this] {showNormal(); });
	connect(ext, &QAction::triggered, this,
		[this] {
			for (auto it : opend_dialog_)
				it->hide();
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
			showNormal();
			break;
		case QSystemTrayIcon::Trigger:
			break;
		case QSystemTrayIcon::MiddleClick:
			break;
		default:
			break;
		}
	};

	connect(tray_icon_, &QSystemTrayIcon::activated, this, cb);*/

	std::function<void(const QColor&)> colselCB = [this](const QColor& color)
	{
		if (current_item_) {
			auto meta = current_item_->MetaData();
			meta->SetColor(color);
			auto bytes = meta->PropertyToStream();
			std::unique_ptr<char> data(new char[bytes.size() + 256] { 0 });
			int r = sprintf(data.get(), "update Memo set PROP=\"%s\" where ID=%u;",
				bytes.data(),
				meta->Id());

			db_->Query(data.get());
			current_item_->Repaint();
			ui.note_wall->repaint();

			if (opend_dialog_.contains(current_item_)) {
				auto memo = meta->converTo<Memo>();
				auto dlg = opend_dialog_.value(current_item_);
				dlg->SetMemo(memo);
				dlg->SetPureStyle(color.name(QColor::HexArgb));
				dlg->repaint();
			}
			repaint();
		}
	};
	connect(color_chunk_, &ColorChunk::ColorSelected, this, colselCB);

	QDir cache(".");
	bool r = cache.mkdir(".cache");
	if (r) {
		cache.setCurrent(".cache");
		SetFileAttributes((LPCWSTR)cache.absolutePath().unicode(), FILE_ATTRIBUTE_HIDDEN);
	}
#if 1
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect();
	shadow_effect->setOffset(0, 0);

	//阴影颜色
	shadow_effect->setColor(QColor(38, 78, 119, 100));

	//阴影半径
	shadow_effect->setBlurRadius(16);

	setGraphicsEffect(shadow_effect);
#endif
}

LxMemo::~LxMemo()
{
	db_->Close();
	delete db_;
}

void LxMemo::onAddMemo()
{
	//Memo memo;
	addMemo(SharedMemo(new Memo()));
}

void LxMemo::onAddGraph()
{
	auto meta = SharedGraph(new GraphNote());
	auto note = ui.note_wall->AddNote();
	note->SetMeta(meta);

	auto time = QDateTime::currentDateTime().toString();

	char buffer[512]{ 0 };
	sprintf(buffer,
		R"(insert into Memo (ID, PROP, IN_RECYCLE, TIME, PPATH, TYPE) values(%u, "%s", 0, "%s", "%s", "GRAPH");)",
		meta->Id(), meta->PropertyToStream().data(), time.toLocal8Bit().data(), path_.path().toLocal8Bit().data());

	//ExecSQL(db_, buffer);
	db_->Query(buffer);

	//QDir dir(tr(".cache"));
	//dir.mkdir(tr("%1").arg(QString::number(meta->Id())));
}

void LxMemo::addMemo(SharedMeta memo)
{
	auto note = ui.note_wall->AddNote();
	note->SetMeta(memo);

	auto time = QDateTime::currentDateTime().toString();

	char buffer[512]{ 0 };
	sprintf(buffer,
		R"(insert into Memo (ID, PROP, IN_RECYCLE, TIME, PPATH, TYPE) values(%u, "%s", 0, "%s", "%s", "FILE");)",
		memo->Id(), memo->PropertyToStream().data(), time.toLocal8Bit().data(), path_.path().toLocal8Bit().data());

	//ExecSQL(db_, buffer);
	db_->Query(buffer);

	QDir dir(tr(".cache"));
	dir.mkdir(tr("%1").arg(QString::number(memo->Id())));
}

void LxMemo::onMemoEdit(const QPoint& pos, NoteWidget* nw)
{
	if (!nw)
		return;

	current_item_ = nw;

	QMenu menu;
	auto delAction = menu.addAction(QIcon(":/LxMemo/icons/recycle-fill.png"), tr("Delete"));

	auto meta = nw->MetaData();
	auto id = meta->Id();
	auto type = meta->type();

	std::function<void(const Path&)> delFolder = [this, id, nw](const Path& path)
	{
		auto sql = tr(R"(select count(*) from Memo where PPATH="%1" and IN_RECYCLE=0)").arg(path.path());

		auto result = db_->Query(sql.toLocal8Bit().data());
		if (result) {
			int rc = result->RowCounts();
			if (rc > 0) {
				int count = std::stoi(any2string(result->Get(0, 0)));

				if (count == 0) {
					char buffer[128]{ 0 };
					sprintf(buffer, "update Memo set IN_RECYCLE=1 where ID=%u", id);
					auto ret = db_->Query(buffer);
					if (ret) {
						ui.note_wall->DeleteNote(nw);
						current_item_ = nullptr;
					}
				}
				else {
					MessageShower::Start("Folder isn't empty", ui.note_wall);
				}
			}
		}
	};

	std::function<void(bool)> delCB = [this, id, delFolder, meta, nw, type](bool)
	{
		auto endpos = mapToGlobal(rect().center() - QPoint(90, 30));
		auto startpos = mapToGlobal(rect().center() - QPoint(90, 30));

		MessageDialog* msg = new MessageDialog(this);
		msg->setAttribute(Qt::WA_DeleteOnClose);

		StartOpacityAnimation(msg, 0, 1);
		StartGeometryAnimation(msg,
			QRect(startpos, QSize(180, 10)),
			QRect(endpos, QSize(180, 60)),
			QEasingCurve::Linear, 100);

		if (type == typeid(Folder).hash_code())
		{
			msg->setText(tr("是否删除该目录？"));
		}
		else {
			msg->setText(QString::fromUtf8("是否删除该文件？"));
		}

		msg->show();
		auto confirm = [this, type, meta, delFolder, nw, id]() {
			if (type == typeid(Folder).hash_code()) {
				auto path = path_;
				auto folder = meta->converTo<Folder>();
				path += folder->name();
				delFolder(path);
			}
			else {
				char buffer[128]{ 0 };
				sprintf(buffer, "update Memo set IN_RECYCLE=1 where ID=%u", id);
				//sprintf(buffer, "delete from Memo where ID=%u", id);
				auto ret = db_->Query(buffer);
				if (ret) {
					//ui.listWidget->takeItem(index.row());
					ui.note_wall->DeleteNote(nw);
					if (opend_dialog_.contains(nw))
						opend_dialog_.value(nw)->close();
					if (cutt_meta_ && cutt_meta_->Id() == id) {
						cutt_meta_ = nullptr;
						cutt_item_ = QModelIndex();
					}
					//repaint();
				}
			}
		};
		connect(msg, &MessageDialog::accept, confirm);
	};

	std::function<void(bool)> showfn = [this, id, pos, nw](bool)
	{
		if (!opend_dialog_.contains(nw))
			onMemoDisplay(pos, nw);
		else
		{
			auto dlg = opend_dialog_.value(nw);
			dlg->close();
			opend_dialog_.remove(nw);
		}
	};

	std::function<void(bool)> colorCB = [this](bool)
	{
		color_chunk_->move(0, height() - color_chunk_->height());
		color_chunk_->resize(width(), color_chunk_->height());
		color_chunk_->show();
	};

	std::function<void(bool)> exportHtml = [this, meta](bool)
	{
		auto memo = meta->converTo<Memo>();
		if (!memo->GetHtml().isEmpty()) {
			this->exportHtml(memo);
		}
		else {
			auto sql = QString("select DATA from Memo where ID=%1").arg(QString::number(memo->Id()));

			auto result = db_->Query(sql.toStdString());
			if (result) {
				auto& row = result->Get(0);
				QString html = QString::fromLocal8Bit(QByteArray::fromBase64(any2chars(row[0])));
				memo->SetHtml(html);
				this->exportHtml(memo);
			}
		}
	};

	std::function<void(bool)> renamefn = [this, meta](bool)
	{
		auto folder = meta->converTo<Folder>();
		addFolder(meta->Id(), folder->name());
	};

	std::function<void(bool)> cutMemofn = [this, meta](bool)
	{
		cutt_meta_ = meta;
		//cutt_item_ = index;
	};

	if (typeid(Folder).hash_code() == type) {
		auto renameAction = menu.addAction(QIcon(":/LxMemo/icons/rename.png"), tr("Rename"));
		connect(renameAction, &QAction::triggered, this, renamefn);
	}
	else if (typeid(Memo).hash_code() == type) {
		auto showAction = menu.addAction(QIcon(":/LxMemo/icons/show.png"), tr("Show"));
		menu.addSeparator();
		auto cutAction = menu.addAction(QIcon(":/LxMemo/icons/cut.png"), tr("Cut"));
		auto exportAction = menu.addAction(QIcon(":/LxMemo/icons/export.png"), tr("Export"));
		auto swapColor = menu.addAction(QIcon(":/LxMemo/icons/eglass-color.png"), tr("Recolor"));

		if (opend_dialog_.contains(nw)) {
			showAction->setIcon(QIcon(":/LxMemo/icons/close_eyes.png"));
			//showAction->setEnabled(false);
		}
		connect(cutAction, &QAction::triggered, this, cutMemofn);
		connect(showAction, &QAction::triggered, this, showfn);
		connect(exportAction, &QAction::triggered, this, exportHtml);
		connect(swapColor, &QAction::triggered, this, colorCB);
	}

	connect(delAction, &QAction::triggered, this, delCB);

	menu.setFixedWidth(100);
	menu.exec(mapToGlobal(pos));
}

void LxMemo::editRecycleItem(const QModelIndex& index)
{
	/*
	if (!index.isValid())
		return;
	auto item = ui.cycleWidget->item(index.row());

	QMenu menu;
	//menu.setFixedWidth(100);
	auto rcvAction = menu.addAction(QIcon(":/LxMemo/icons/recovery.png"), tr("Rrecover"));
	auto delAction = menu.addAction(QIcon(":/LxMemo/icons/close.png"), tr("Delete"));

	auto memo = qvariant_cast<SharedMemo>(item->data(0));
	uint32_t id = memo->Id();

	std::function<void(bool)> delCB = [this, index, id](bool)
	{
		char buffer[512]{ 0 };
		sprintf(buffer, "delete from Memo where ID=%u;", id);
		ExecSQL(db_, buffer);
		ui.cycleWidget->takeItem(index.row());

		///删除该Memo同时删除所有的图片资源
		QDir dir(tr(".cache/%1").arg(QString::number(id)));
		if (dir.exists()) {
			dir.removeRecursively();
		}
	};

	std::function<void(bool)> rcvCB = [this, index, id](bool)
	{
		char buffer[128]{ 0 };
		sprintf(buffer, "update Memo set IN_RECYCLE=0 where ID=%u", id);

		ExecSQL(db_, buffer);
		ui.cycleWidget->takeItem(index.row());
	};

	connect(delAction, &QAction::triggered, this, delCB);
	connect(rcvAction, &QAction::triggered, this, rcvCB);

	auto p = ui.cycleWidget->cursor().pos();
	p = mapFromGlobal(p);
	menu.exec(mapToGlobal(p - QPoint(50, 0)));
	*/
}

void LxMemo::onRecycleItemEdit(const QModelIndex& index)
{
	/*
	auto item = ui.cycleWidget->item(index.row());
	auto p = ui.cycleWidget->cursor().pos();
	auto rect = ui.cycleWidget->visualItemRect(item);
	p = mapFromGlobal(p);

	_cc->hide();

	auto tR = rect.topRight();
	auto r = QRect(tR - QPoint(25, -50), tR - QPoint(5, -60));
	if (r.contains(p)) {
		editRecycleItem(index);
	}
	*/
}

void LxMemo::onImportMemo()
{
	auto htmlFile = QFileDialog::getOpenFileName(nullptr, tr("Open"), QString(), "*.html;*.txt");
	if (htmlFile.isEmpty())
		return;

	QFile f(htmlFile);
	f.open(QIODevice::ReadOnly);
	if (f.isOpen()) {
		auto content = f.readAll();
		f.close();

		auto memo = SharedMemo(new Memo());
		memo->SetHtml(content);
		addMemo(memo);

		auto content_base = QString(content).toLocal8Bit().toBase64();
		auto snap = memo->Snapshot().toLocal8Bit().toBase64();
		std::unique_ptr<char> data(new char[content_base.length() + snap.length() + 256] { 0 });
		int r = sprintf(data.get(), R"(update Memo set DATA="%s", SNAPSHOT="%s" where ID=%u;)",
			content_base.data(),
			snap.data(),
			memo->Id());

		//ExecSQL(db_, data.get());
		db_->Query(data.get());
		//onUpdateMemo(memo->Id());
	}
}

/// <summary>
/// 对memo进行编辑，忽略文件夹节点
/// </summary>
/// <param name="index"></param>
void LxMemo::onItemEdit(const QPoint& pos, NoteWidget* nw)
{
	if (nw) {
		auto type = nw->MetaData()->type();
		if (typeid(Memo).hash_code() == type) {
			auto rect = nw->Geometry();
			color_chunk_->hide();

			auto tR = rect.topRight();
			auto r = QRect(tR - QPoint(30, -8), tR - QPoint(9, -21));
			if (r.contains(pos)) {
				//auto pos = ui.listWidget->mapToGlobal(r.bottomLeft());
				onMemoEdit(pos, nw);
			}
		}
	}
}

///在编辑memo后，更新主界面memo的snapshot
void LxMemo::onUpdateMemo(MemoDialog* mdlg)
{
	auto note = opend_dialog_.key(mdlg);
	if (note)
		note->Repaint();
	ui.note_wall->update();
}

void LxMemo::onMemoClosed()
{
	auto obj = qobject_cast<MemoDialog*>(sender());
	//auto w = obj->Widget<MemoDialog*>();
	auto note = opend_dialog_.key(obj);
	if (note)
		opend_dialog_.remove(note);
	//delete w;

	/// <summary>
	/// 如果当前的主界面已经隐藏，且无打开的memo，则关闭软件
	/// </summary>
	if (opend_dialog_.isEmpty()) {
		//if (isHidden())
		//    close();
		show();
	}
}

void LxMemo::onBack()
{
	if (!path_.isRoot()) {
		path_ = path_.parent();
		ui.label->setText(path_.path());

		//ui.listWidget->clear();
		ui.note_wall->WipeUp();
		queryData(path_);

		if (path_.isRoot())
			ui.button_back->setEnabled(false);
	}
}

void LxMemo::onForward()
{
}

void LxMemo::addFolder(uint32_t id, const QString& old_name)
{
	auto endpos = mapToGlobal(rect().center() - QPoint(110, 30));
	auto startpos = mapToGlobal(rect().center() - QPoint(110, 30));

	FolderCreator* creator = new FolderCreator(old_name, this);
	creator->setAttribute(Qt::WA_DeleteOnClose);

	StartOpacityAnimation(creator, 0, 1);
	StartGeometryAnimation(creator,
		QRect(startpos, QSize(220, 10)),
		QRect(endpos, QSize(220, 60)),
		QEasingCurve::Linear, 100);
	creator->show();

	auto call = [this, creator, id, old_name]()
	{
		auto name = creator->getText();

		if (id == 0) {
			if (!name.isEmpty()) {
				auto folder = SharedFolder(new Folder());
				folder->SetTime(QDateTime::currentDateTime().toString());
				folder->setName(name);

				char buffer[512]{ 0 };
				auto time = QDateTime::currentDateTime().toString();
				auto path = path_;
				path += name;

				sprintf(buffer,
					R"(insert into Memo (ID, PROP, IN_RECYCLE, TIME, PATH, PPATH, TYPE) values(%u,"%s", 0, "%s", "%s", "%s", "FOLDER");)",
					folder->Id(), folder->PropertyToStream().data(), time.toLocal8Bit().data(), path.path().toLocal8Bit().data(),
					path_.path().toLocal8Bit().data());

				auto ret = db_->Query(buffer);
				if (ret) {
					auto note = ui.note_wall->AddNote();
					note->SetMeta(folder);
					note->Repaint();
					ui.note_wall->repaint();
				}
			}
		}
		else {
			onEditFinished(id, old_name, name);
			auto folder = current_item_->MetaData()->converTo<Folder>();
			folder->setName(name);
			current_item_->Repaint();
			ui.note_wall->repaint();
		}
	};

	connect(creator, &FolderCreator::windowClosed, call);
}

void LxMemo::onAddFolder()
{
	addFolder(0, QString());
}

void LxMemo::onEditFinished(uint32_t id, const QString& last, const QString& curr)
{
	auto oldpath = path_;
	oldpath += last;

	auto path = path_;
	path += curr;

	auto sql = tr("update Memo set PATH='%1' where ID=%2;").arg(path.path().data(), QString::number(id));
	auto ret = db_->Query(sql.toLocal8Bit().toStdString());
	if (ret) {
		sql = tr("update Memo set PPATH=replace(PPATH, '%1', '%2');")
			.arg(oldpath.path(), path.path());
		db_->Query(sql.toLocal8Bit().data());
	}
}

void LxMemo::loadRecyleMemo(SharedMeta memo)
{
	/*
	QListWidgetItem* item = new QListWidgetItem();
	item->setData(0, QVariant::fromValue(memo));
	item->setData(Qt::UserRole, memo->Id());
	ui.cycleWidget->insertItem(0, item);
	*/
}

void LxMemo::onReadDataFromDB(const Row& row)
{
	uint32_t id = std::stoull(any2string(row[0]));
	QByteArray prop = any2chars(row[1]);
	QString snapshot = QString::fromLocal8Bit(QByteArray::fromBase64(any2chars(row[2])));
	QString time = QString::fromLocal8Bit(any2chars(row[3]));
	QString type = any2chars(row[4]);
	QString path = QString::fromLocal8Bit(any2chars(row[5]));

	if (type == "FOLDER") {
		auto meta = SharedFolder(new Folder(id));
		QDir dir(path);
		meta->setName(dir.dirName());
		meta->SetTime(time);
		meta->StreamToProperty(prop);

		auto note = ui.note_wall->AddNote();
		note->SetMeta(meta);
	}
	else if (type == "GRAPH") {
		auto meta = SharedGraph(new GraphNote(id));
		auto note = ui.note_wall->AddNote();
		note->SetMeta(meta);
	}
	else {
		auto memo = SharedMemo(new Memo(id));
		memo->SetSnapshot(snapshot);
		memo->StreamToProperty(prop);
		memo->SetTime(time);

		auto note = ui.note_wall->AddNote();
		note->SetMeta(memo);
	}
}

void LxMemo::queryData(const Path& path)
{
	auto sql = tr(R"(select ID, PROP, SNAPSHOT, TIME, TYPE, PATH from Memo where PPATH="%1" and IN_RECYCLE=0;)").arg(path.path());
	auto result = db_->Query(sql.toLocal8Bit().data());
	int row = result->RowCounts();
	for (int i = 0; i < row; i++)
		onReadDataFromDB(result->Get(i));
}

void LxMemo::exportHtml(SharedMemo memo)
{
	auto htmlFile = QFileDialog::getSaveFileName(nullptr, tr("Save"), QString(), tr("HTML(*.html);;MarkDown(*.md)"));
	if (htmlFile.isEmpty())
		return;

	QFileInfo finfo(htmlFile);
	auto suffx = finfo.suffix();

	QFile file(htmlFile);
	file.open(QIODevice::WriteOnly);
	if (file.isOpen()) {
		if (suffx == "md")
			file.write(memo->GetMarkdown().toUtf8());
		else
			file.write(memo->GetHtml().toUtf8());
		file.close();
	}
}

void LxMemo::closeEvent(QCloseEvent* e)
{
	//if (tray_icon_->isVisible()) {
	//    hide();
	//    e->ignore();
	//}
	//else {
	//    e->accept();
	//}
	e->accept();
	//return NoFrameWidget::closeEvent(e);
}

void LxMemo::initDB()
{
	db_ = DBFactory::Create("sqlite3");
	db_->Open("memo.db");

	auto sql = R"(
        CREATE TABLE IF NOT EXISTS Memo(
            ID BIGINT PRIMARY KEY NOT NULL,
            DATA TEXT,
            PROP TEXT,
            IN_RECYCLE INTEGER,
            SNAPSHOT TEXT,
            TIME TEXT,
            PATH TEXT,
            PPATH TEXT,
            TYPE TEXT);
    )";
	db_->Query(sql);

	queryData(Path("/"));
}

int onReadCycleMemo(void* lx, int rows, char** rowData, char** colNames)
{
	/*
	auto lxmemo = (LxMemo*)lx;

	uint32_t id = std::stoull(rowData[0]);
	QByteArray prop = rowData[1];
	QString snapshot = QString::fromLocal8Bit(QByteArray::fromBase64(rowData[2]));
	QString time = QString::fromLocal8Bit(rowData[3]);

	auto memo = SharedMemo(new Memo(id));
	memo->SetSnapshot(snapshot);
	memo->StreamToProperty(prop);
	memo->SetTime(time);

	lxmemo->loadRecyleMemo(memo);
	*/
	return 0;
}

void LxMemo::initRecycleMemo()
{
	const char* sql = "select ID, PROP, SNAPSHOT, TIME from Memo where IN_RECYCLE=1;";
	//ExecSQL(db_, sql, onReadCycleMemo, this);
	auto result = db_->Query(sql);
	if (result) {
		//onReadCycleMemo();
	}
}

MemoDialog* LxMemo::showMemo(SharedMemo memo)
{
	MemoDialog* dlg = new MemoDialog(this);
	dlg->SetMemo(memo);

	srand(time(nullptr));
	auto TR = rect().topRight();
	TR = mapToGlobal(TR);

	TR.rx() += rand() % 200;
	TR.ry() += rand() % 150;
	dlg->move(TR);
	dlg->show();

	connect(dlg, &MemoDialog::MemoUpdate, this, &LxMemo::onUpdateMemo, Qt::QueuedConnection);
	connect(dlg, &MemoDialog::windowClosed, this, &LxMemo::onMemoClosed);

	StartOpacityAnimation(dlg, 0, 0.95);

	return dlg;
}

void LxMemo::onMemoDisplay(const QPoint& pos, NoteWidget* nw)
{
	if (!nw)
		return;

	auto meta = nw->MetaData();
	if (typeid(Folder).hash_code() == meta->type())
	{
		auto folder = meta->converTo<Folder>();
		path_ += folder->name();

		ui.label->setText(path_.path());
		ui.note_wall->WipeUp();
		ui.button_back->setEnabled(true);

		queryData(path_);

		cutt_item_ = QModelIndex();
	}
	else if (typeid(GraphNote).hash_code() == meta->type()) {
		//auto isShow = opend_dialog_.contains(nw);
		//if (isShow)
		//    return;
		auto gn = meta->converTo<GraphNote>();
		if (gn->Data().isNull()) {
			auto sql = QString("select DATA from Memo where ID=%1").arg(QString::number(gn->Id()));

			auto result = db_->Query(sql.toStdString());
			if (result) {
				if (result->RowCounts() > 0) {
					auto& row = result->Get(0);
					auto content = QByteArray::fromBase64(any2chars(row[0]));
					gn->SetData(content);
				}
				//opend_dialog_.insert(nw, showMemo(memo));
			}
		}
		Canvas* canvas = new Canvas();
		canvas->SetMeta(gn);
		canvas->SetMetaSaver(saver_);
		canvas->setAttribute(Qt::WA_DeleteOnClose);
		canvas->show();
	}
	else {
		auto isShow = opend_dialog_.contains(nw);
		if (isShow)
			return;
		auto memo = meta->converTo<Memo>();
		if (memo->GetHtml().isEmpty()) {
			auto sql = QString("select DATA from Memo where ID=%1").arg(QString::number(memo->Id()));

			auto result = db_->Query(sql.toStdString());
			if (result) {
				auto& row = result->Get(0);
				QString html = QString::fromLocal8Bit(QByteArray::fromBase64(any2chars(row[0])));
				memo->SetHtml(html);
				opend_dialog_.insert(nw, showMemo(memo));
			}
		}
		else
			opend_dialog_.insert(nw, showMemo(memo));
	}
}

//bool LxMemo::eventFilter(QObject* object, QEvent* event)
//{
//    if (object == ui.listWidget) {
//        if (event->type() == QEvent::Resize) {
//            if (!color_chunk_->isHidden()) {
//                QResizeEvent* ev = (QResizeEvent*)event;
//                color_chunk_->move(0, ui.listWidget->height());
//                color_chunk_->resize(width(), color_chunk_->height());
//            }
//        }
//    }
//    return NoFrameWidget::eventFilter(object, event);
//}