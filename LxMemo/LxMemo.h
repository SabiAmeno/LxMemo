#pragma once

#include <QDir>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QEasingCurve>

#include "ui_LxMemo.h"
#include "lucencydialog.h"
#include "Memo.h"
#include "ColorChunk.h"
#include "MemoDialog.h"

//#include "sqlite3.h"
#include "DBFactory.h"
#include "QueryResult.h"
#include "NoFrameWidget.h"
#include "note.h"

#include "MetaSaver.h"

class Path
{
public:
	Path(const QString& s) {
		_paths = s.split('/', Qt::SkipEmptyParts);
	}
	Path(const QStringList& p) : _paths(p) {}

	Path& operator+(const QString& d) { _paths.append(d); return *this; }
	//Path& operator+=(const Path& p) { _paths.append("/" + p.path()); return *this; }
	Path& operator+=(const QString& p) { _paths.append(p); return *this; }
	Path parent()
	{
		auto p = _paths;
		p.pop_back();

		return Path(p);
	}
	bool isRoot() const { return _paths.isEmpty(); }
	QString path() const { return "/" + _paths.join('/'); }
private:
	QStringList _paths;
};

class LxMemo : public NoFrameWidget
{
	Q_OBJECT

public:
	LxMemo(QWidget* parent = nullptr);
	~LxMemo();

	MetaSaver* saver_{ nullptr };

public slots:
	//void onWindowClosed();
private slots:
	void onEditMemo(const QPoint& pos, NoteWidget* nw);
	void onRecycleItemEdit(const QModelIndex&);
	void onDeleteMemo(const QPoint& pos, NoteWidget* nw);
	void onItemEdit(const QPoint& pos, NoteWidget* nw);
	void onUpdateMemo(MemoDialog* mdlg);
	void onMemoClosed();

	void onBack();
	void onForward();
	void onAddFolder();

	void onAddMemo();
	void onAddGraph();

	void onEditFinished(uint32_t id, const QString& last, const QString& curr);
protected:
	//virtual bool eventFilter(QObject* object, QEvent* event) override;
	virtual void closeEvent(QCloseEvent* e) override;
private:
	void initDB();
	void initRecycleMemo();
	void editItem(const QPoint& pos, NoteWidget* nw);
	void editRecycleItem(const QModelIndex& index);
	void addMemo(SharedMeta memo);
	void queryData(const Path& path);

	void exportHtml(SharedMemo memo);
	void addFolder(uint32_t id, const QString& name);
	void onReadDataFromDB(const Row& row);

	void loadRecyleMemo(SharedMeta memo);
	MemoDialog* showMemo(SharedMemo memo);

private:
	Ui::LxMemoClass ui;

	DB* db_ = nullptr;

	//QGraphicsOpacityEffect* effect_;
	//QPropertyAnimation* _opacityAni;
	QSystemTrayIcon* tray_icon_;

	QMap<NoteWidget*, MemoDialog*> opend_dialog_;
	//LucencyDialog* fontDlg;
	ColorChunk* color_chunk_;
	//QListWidgetItem* current_item_{ nullptr };
	NoteWidget* current_item_{ nullptr };

	//bool start_slide_{ false };
	//QPoint slide_position_;

	Path path_{ "/" };

	QModelIndex cutt_item_;
	SharedMeta cutt_meta_ = nullptr;
};
