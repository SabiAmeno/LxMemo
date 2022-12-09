#include "MemoDialog.h"
#include <QFontDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QClipBoard>
#include <QMimeData>
#include <QImageReader>
#include <QScrollBar>
#include <QMenu>
#include "LxMemo.h"
#include "db.h"
//#include "ColorChunk.h"

MemoDialog::MemoDialog(LxMemo* parent)
	: NoFrameWidget(nullptr), lxmemo_(parent)
{
	ui.setupUi(this);

	ui.widget->SetIcon(":/LxMemo/icons/memo_note.png");

	ui.textEdit->document()->setIndentWidth(4);
	ui.textEdit->viewport()->installEventFilter(this);
	//ui.textEdit->installEventFilter(this);
	ui.textEdit->setUndoRedoEnabled(true);

	connect(ui.textEdit, &QTextEdit::customContextMenuRequested, this, &MemoDialog::onMenuPop);
	connect(ui.textEdit, &QTextEdit::selectionChanged, this, &MemoDialog::onSelectChanged);

	searcher_ = new Searcher(ui.textEdit->document(), this);
	searcher_->hide();

	edit_tool_ = new EditTools(ui.textEdit, this);
	//_editTool->setWindowFlag(Qt::ToolTip);

	edit_tool_->hide();

	save_timer_.setInterval(500);

	connect(edit_tool_, &EditTools::changeBold, this, &MemoDialog::onTextBold);
	connect(edit_tool_, &EditTools::changeItalic, this, &MemoDialog::onTextItalic);
	connect(edit_tool_, &EditTools::changeUnderline, this, &MemoDialog::onTextUnderline);
	connect(edit_tool_, &EditTools::changeStrikeout, this, &MemoDialog::onTextStrikeout);
	connect(edit_tool_, &EditTools::changeFont, this, &MemoDialog::onTextFontChanged);
	connect(edit_tool_, &EditTools::changeFontSize, this, &MemoDialog::onTextFontSize);
	connect(edit_tool_, &EditTools::changeFontColor, this, &MemoDialog::onTextColorChanged);
	connect(edit_tool_, &EditTools::changeBackgroundColor, this, &MemoDialog::onTextBkColorChanged);
	connect(edit_tool_, &EditTools::changeFontUp, this, &MemoDialog::onTextColorUp);
	connect(edit_tool_, &EditTools::changeFontDown, this, &MemoDialog::onTextColorDown);

	//自定义标题栏行为
	connect(ui.widget, &TitleBar::closeButtonClicked, this, [this] { close(); });
	connect(ui.widget, &TitleBar::minimumButtonClicked, this, [this] {showMinimized(); });
	connect(ui.widget, &TitleBar::positionChanged, this, [this](const QPoint& diff) {move(pos() + diff); });

	connect(ui.textEdit->document(), &QTextDocument::contentsChanged, this, &MemoDialog::updateLines);
	connect(&save_timer_, &QTimer::timeout, this, &MemoDialog::onMemoSave);

	size_selector_dialog_ = new LucencyDialog(this);
	size_selector_ = new SizeSelector(size_selector_dialog_);
	size_selector_dialog_->SetWidget(size_selector_);
	size_selector_dialog_->SetFixedEnable(false);
	size_selector_dialog_->SetPureStyle("rgb(235,229,209)");
	size_selector_dialog_->SetAttribute(~WA_MINIMUM_WINDOW & ~WA_MAXIMUM_WINDOW);
	size_selector_dialog_->resize(230, 80);
	size_selector_dialog_->hide();

	connect(size_selector_, &SizeSelector::SelectFinished, this, &MemoDialog::onImageSelected);
	connect(ui.textEdit->verticalScrollBar(), &QScrollBar::valueChanged, ui.count_edit->verticalScrollBar(), &QScrollBar::setValue);
	connect(ui.count_edit->verticalScrollBar(), &QScrollBar::valueChanged, ui.textEdit->verticalScrollBar(), &QScrollBar::setValue);

	init();

	save_timer_.start();
}

MemoDialog::~MemoDialog()
{}

void MemoDialog::SetMemo(SharedMemo memo)
{
	memo_ = memo;

	auto title = QString::fromLocal8Bit("ID:%1").arg(memo->Id());
	SetTitle(title);
	ui.widget->SetIcon(":/LxMemo/icons/memo_note.png");
	SetPureStyle(memo->GetColor().name(QColor::HexArgb));

	if (MemoType::kMemoHtml == memo_->GetMemoType())
		ui.textEdit->setHtml(memo_->GetRawData());
	else
		ui.textEdit->setPlainText(memo_->GetRawData());

	updateLines();
}

uint32_t MemoDialog::MemoId()
{
	if (!memo_) return 0;

	return memo_->Id();
}

void MemoDialog::SetTitle(const QString& title)
{
	ui.widget->SetTitle(title);
}

void MemoDialog::onWindowClosed()
{
	save_timer_.stop();
	searcher_->ResetSearcher();
	edit_tool_->close();

	//keep_.store(false);
	//cv_.notify_all();
}

void MemoDialog::closeEvent(QCloseEvent* e)
{
	onWindowClosed();

	emit windowClosed();
	NoFrameWidget::closeEvent(e);
}

void MemoDialog::keyPressEvent(QKeyEvent* e)
{
	auto ctrl = e->modifiers();
	auto key = e->key();

	if (Qt::ControlModifier == ctrl)
	{
		if (Qt::Key_S == key/* && !save_right_*/) {
			//save_right_ = true;
			//cv_.notify_one();
			onMemoSave();
		}
		if (Qt::Key_F == key) {
			onTextSearch(true);
		}
		if (Qt::Key_K == key) {
			auto cursor = ui.textEdit->textCursor();
			auto format = cursor.charFormat();

			if (format.verticalAlignment() != QTextCharFormat::AlignNormal) {
				format.setVerticalAlignment(QTextCharFormat::AlignNormal);
				cursor.insertText(" ");

				QTextCursor tempCursor(cursor);
				tempCursor.setPosition(cursor.selectionStart() - 1);
				tempCursor.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
				tempCursor.mergeCharFormat(format);
			}
		}
		if (Qt::Key_U == key) {
			auto cursor = ui.textEdit->textCursor();
			auto format = cursor.charFormat();
			bool chk = format.verticalAlignment() == QTextCharFormat::AlignNormal;
			onTextColorUp(chk);
		}
		if (Qt::Key_D == key) {
			auto cursor = ui.textEdit->textCursor();
			auto format = cursor.charFormat();
			bool chk = format.verticalAlignment() == QTextCharFormat::AlignNormal;
			onTextColorDown(chk);
		}
	}

	return NoFrameWidget::keyPressEvent(e);
}


void MemoDialog::updateLines()
{
	ui.count_edit->clear();
	auto count = ui.textEdit->document()->lineCount();
	QString count_string;
	for (int i = 0; i < count; i++)
		count_string += QString::number(i + 1) + "\n";
	ui.count_edit->setPlainText(count_string);
}

void MemoDialog::zoom(bool zoom_in)
{
	if (zoom_in) {
		ui.textEdit->zoomIn(2);
		ui.count_edit->zoomIn(2);
	} else {
		ui.textEdit->zoomOut(2);
		ui.count_edit->zoomOut(2);
	}
}


bool MemoDialog::eventFilter(QObject* watch, QEvent* event)
{
	switch (event->type())
	{
		case QEvent::Wheel:
		{
		    auto e = (QWheelEvent*)event;
		    auto ctrl = e->modifiers();

			QPoint numPixels = e->pixelDelta();
			QPoint numDegrees = e->angleDelta() / 8;

			if (Qt::ControlModifier == ctrl) {
				if (!numPixels.isNull()) {
					zoom(numPixels.y() > 0);
				}
				else if (!numDegrees.isNull()) {
					QPoint numSteps = numDegrees / 15 * 40;
					zoom(numSteps.y() > 0);
				}
			}
		    break;
		}
	case QEvent::MouseButtonPress:
	{
		edit_tool_->hide();
		text_selected_ = false;
		break;
	}
	case QEvent::MouseButtonRelease:
	{
		auto e = (QMouseEvent*)event;
		if (e->button() == Qt::LeftButton) {
			if (text_selected_) {
				auto cursor = ui.textEdit->textCursor();
				auto imp_cursor = ui.textEdit->cursor();
				auto pos = imp_cursor.pos() - QPoint(-10, 80);
				edit_tool_->UpdateTool(cursor.charFormat());
				edit_tool_->move(pos);
				edit_tool_->show();
			}
			else
				edit_tool_->hide();
		}
		break;
	}
	case QEvent::MouseButtonDblClick:
	{
		auto cursor = ui.textEdit->textCursor();
		auto fmt = cursor.charFormat();
		switch (fmt.objectType())
		{
			///FIXME: 查看图片
		case QTextFormat::ImageObject:
		{
			auto prop = fmt.properties();
			for (auto& it : prop) {
				auto k = it;
			}

			break;
		}
		default:break;
		}
		break;
	}
	case QEvent::MouseMove: {
		if (!edit_tool_->isHidden()) {
			auto rt = edit_tool_->geometry();
			rt.adjust(-40, -40, 40, 40);

			auto pos = ((QMouseEvent*)event)->pos();
			pos = mapToGlobal(pos);

			if (!rt.contains(pos)) {
				edit_tool_->hide();
			}
		}
		break;
	}
	default:
		break;
	}

	return NoFrameWidget::eventFilter(watch, event);
}

void MemoDialog::resizeEvent(QResizeEvent* event)
{
	if (!searcher_->isHidden()) {
		searcher_->resize(ui.textEdit->width(), 30);
		searcher_->move(-3, height() - 35);
	}
	NoFrameWidget::resizeEvent(event);
}

void MemoDialog::focusOutEvent(QFocusEvent* event)
{
	NoFrameWidget::focusOutEvent(event);
}

void MemoDialog::onTextBold(bool checked)
{
	changeProp([&](QTextCharFormat& format) {format.setFontWeight(checked ? QFont::Bold : QFont::Normal); });
}

void MemoDialog::onTextItalic(bool checked)
{
	changeProp([&](QTextCharFormat& format) {format.setFontItalic(checked); });
}

void MemoDialog::onTextUnderline(bool checked)
{
	changeProp([&](QTextCharFormat& format) {format.setFontUnderline(checked); });
}

void MemoDialog::onTextStrikeout(bool checked)
{
	changeProp([&](QTextCharFormat& format) {format.setFontStrikeOut(checked); });
}

void MemoDialog::onTextFontSize(const QString& s)
{
	if (s.isEmpty())
		return;

	auto font = ui.textEdit->textCursor().charFormat().font();
	font.setPointSize(s.toInt());
	changeProp([&](QTextCharFormat& format) {format.setFont(font); });
}

void MemoDialog::onInsertImage()
{
	/* auto cursor = ui.textEdit->textCursor();*/
	auto imgFile = QFileDialog::getOpenFileName(nullptr, tr("Image"), QString(), tr("JPG (*.jpeg *.jpg);;PNG (*.png);;XMP (*.xpm)"));
	if (!imgFile.isEmpty()) {
		size_selector_->Init(imgFile);
		size_selector_dialog_->show();
	}
}

void MemoDialog::onSymbolChecked(bool checked)
{
	//if (checked) {
	auto cursor = ui.textEdit->textCursor();
	auto list = cursor.currentList();

	QTextListFormat listFormat;

	listFormat.setStyle(QTextListFormat::ListDisc);
	listFormat.setIndent(3);

	if (!list) {
		list = cursor.createList(listFormat);
		list->setFormat(listFormat);
	}
	else {
		QTextListFormat listFormat;
		listFormat.setStyle(QTextListFormat::ListCircle);
		listFormat.setIndent(6);
		auto list = cursor.createList(listFormat);
		list->setFormat(listFormat);
	}
}

void MemoDialog::onMenuPop(const QPoint& pos)
{
	QMenu menu;

	auto copyAction = menu.addAction(QIcon(":/LxMemo/icons/copy.png"), tr("Copy"));
	auto cutAction = menu.addAction(QIcon(":/LxMemo/icons/cut.png"), tr("Cut"));
	auto textPasteAction = menu.addAction(QIcon(":/LxMemo/icons/paste.png"), tr("Paste"));
	menu.addSeparator();

	auto findAction = menu.addAction(QIcon(":/LxMemo/icons/search.png"), tr("&Find"));
	auto insertImgAction = menu.addAction(QIcon(":/LxMemo/icons/picture.png"), tr("Insert"));
	auto addSymbolAction = menu.addAction(QIcon(":/LxMemo/icons/symbol.png"), tr("Bullets"));

	auto colorAction = menu.addAction(QIcon(":/LxMemo/icons/highlight.png"), tr("Highlight"));
	auto clearAction = menu.addAction(QIcon(":/LxMemo/icons/clear_format.png"), tr("Clear Format"));
	//auto addUrlAction = menu.addAction(QIcon(""), "Add URL");
	menu.addSeparator();
	auto selAllAction = menu.addAction(QIcon(":/LxMemo/icons/selall.png"), tr("Select All"));

	auto cursor = ui.textEdit->textCursor();
	if (!cursor.hasSelection())
		copyAction->setEnabled(false);

	std::function<void(bool)> selfn = [this](bool)
	{
		ui.textEdit->selectAll();
	};

	std::function<void(bool)> clrfn = [this](bool)
	{
		changeProp([&](QTextCharFormat& format) {
			auto background = format.background();
		auto fore = format.foreground();
		if (background.color() != Qt::darkGreen)
			format.setBackground(Qt::darkGreen);
		else
			format.setBackground(Qt::transparent);
		format.setForeground(fore);
			});
	};

	std::function<void(bool)> clearfn = [this](bool)
	{
		auto cursor = ui.textEdit->textCursor();
		auto bfmt = cursor.blockFormat();
		bfmt.setBackground(Qt::transparent);
		cursor.mergeBlockFormat(bfmt);

		auto cfmt = cursor.charFormat();
		cfmt.setBackground(Qt::transparent);
		cursor.mergeCharFormat(cfmt);
	};

	std::function<void(bool)> cutfn = [this](bool)
	{
		auto clip = qApp->clipboard();
		auto cursor = ui.textEdit->textCursor();
		auto text = cursor.selection();

		QMimeData* mime = new QMimeData();
		mime->setHtml(text.toHtml());
		clip->setMimeData(mime);
		cursor.removeSelectedText();
	};

	std::function<void(bool)> copyfn = [this](bool)
	{
		auto cursor = ui.textEdit->textCursor();
		auto text = cursor.selection();

		QMimeData* mime = new QMimeData();
		mime->setHtml(text.toHtml());
		qApp->clipboard()->setMimeData(mime);
	};

	std::function<void(bool)> pastetextfn = [this](bool)
	{
		auto cursor = ui.textEdit->textCursor();
		auto clip = qApp->clipboard();
		auto mime = clip->mimeData();
		if (mime->hasText())
			cursor.insertText(mime->text(), QTextCharFormat());
		else if (mime->hasHtml())
			cursor.insertHtml(mime->html());
		else if (mime->hasImage()) {
			auto imgid = GenId();
			auto imgname = tr(".cache/%1/%2.png").arg(QString::number(memo_->Id()), QString::number(imgid));
			auto img = qvariant_cast<QImage>(mime->imageData());
			img.save(imgname);
			//QTextImageFormat fmt;
			//fmt.setWidth(img.width());
			//fmt.setHeight(img.height());
			//fmt.setName(imgname);
			//cursor.insertImage(fmt);

			size_selector_->Init(imgname);
			size_selector_dialog_->show();
		}
	};

	/*
	std::function<void(bool)> addurlfn = [this](bool)
	{
		//const char* url = "https://www.baidu.com";
		const char* url = R"(<a href="url">https://www.baidu.com</a>)";
		auto cursor = ui.textEdit->textCursor();
		cursor.insertHtml(url);
	};
	*/
	connect(colorAction, &QAction::triggered, this, clrfn);
	connect(clearAction, &QAction::triggered, this, clearfn);
	connect(selAllAction, &QAction::triggered, this, selfn);
	connect(copyAction, &QAction::triggered, this, copyfn);
	connect(cutAction, &QAction::triggered, this, cutfn);
	connect(textPasteAction, &QAction::triggered, this, pastetextfn);

	connect(insertImgAction, &QAction::triggered, this, &MemoDialog::onInsertImage);
	connect(findAction, &QAction::triggered, this, &MemoDialog::onTextSearch);
	connect(addSymbolAction, &QAction::triggered, this, &MemoDialog::onSymbolChecked);
	//connect(addUrlAction, &QAction::triggered, this, addurlfn);

	auto npos = mapToGlobal(pos);
	menu.exec(npos);
}

void MemoDialog::onSelectChanged()
{
	auto cursor = ui.textEdit->textCursor();
	if (cursor.hasSelection())
		text_selected_ = true;
}

void MemoDialog::onTextSearch(bool check)
{
	//if (!check)
	//    _searcher->hide();
	//else {
	searcher_->resize(ui.textEdit->width(), 30);
	searcher_->move(-3, height() - 35);
	searcher_->show();
	searcher_->setFocus();
	//}
}

void MemoDialog::onImageSelected(const QTextImageFormat& format)
{
	size_selector_dialog_->hide();
	//_selector->hide();
	//此处可插入原图，当前对图片进行了缩放处理
	auto cursor = ui.textEdit->textCursor();
	cursor.insertImage(format, QTextFrameFormat::InFlow);
}

void MemoDialog::onTextFontChanged(const QFont& font)
{
	changeProp([&](QTextCharFormat& format) {format.setFont(font); });
}

void MemoDialog::onTextColorChanged(const QColor& color)
{
	changeProp([&](QTextCharFormat& format) {format.setForeground(color); });
}

void MemoDialog::onTextBkColorChanged(const QColor& color)
{
	changeProp([&](QTextCharFormat& format) {format.setBackground(color); });
}

void MemoDialog::onTextColorUp(bool checked)
{
	changeProp([&](QTextCharFormat& format) {
		auto align = checked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal;
	format.setVerticalAlignment(align);
		});
}

void MemoDialog::onTextColorDown(bool checked)
{
	changeProp([&](QTextCharFormat& format) {
		auto align = checked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal;
	format.setVerticalAlignment(align);
		});
}

void MemoDialog::onMemoSave()
{
	//FIXME:
	//此处每有变动都会调用，是否会太频繁，
	//如果使用定时器是否会好一些？
	if (ui.textEdit->document()->isModified()) {
		ui.textEdit->document()->setModified(false);

		auto time = QDateTime::currentDateTime().toString();
		QString contents;

		if (kMemoHtml == memo_->GetMemoType()) {
			contents = ui.textEdit->document()->toHtml();
		}
		else {
			contents = ui.textEdit->document()->toPlainText();
		}

		memo_->SetRawData(contents);

		memo_->SetTime(time);

		Event* event = new Event();
		event->meta = memo_;
		event->snapshot = memo_->Snapshot().toLocal8Bit().toBase64();
		event->contents = contents.toLocal8Bit().toBase64();
		event->time = time;

		lxmemo_->saver_->Push(event);
	}
}

void MemoDialog::init()
{
	//thread_ = new std::thread(std::bind(&MemoDialog::saveInThread, this));
}

void MemoDialog::changeProp(auto cb)
{
	///Q:从左往右和从右往左选中文本，产生的效果不同
	///我也很纳闷，是QTextEdit自身的问题还是实现的问题

	auto cursor = ui.textEdit->textCursor();
	auto start = cursor.selectionStart();
	auto end = cursor.selectionEnd();
	auto doc = ui.textEdit->document();
	auto firstBlock = doc->findBlock(start);
	auto lastBlock = doc->findBlock(end);
	int i = firstBlock.blockNumber();
	int j = lastBlock.blockNumber();

	///将所有的编辑当做一个事务
	//std::map<std::tuple<int, int, QTextBlock*>, QTextFragment> modify;
	cursor.beginEditBlock();
	for (j; j >= i; j--)
	{
		auto block = doc->findBlockByNumber(j);
		if (!block.isValid())
			continue;

		auto it = block.begin();
		std::map<std::pair<int, int>, QTextFragment> modify;
		for (it; it != block.end(); ++it) {
			auto frag = it.fragment();
			int from = frag.position();
			int to = frag.position() + frag.length();

			if (to < start) continue;
			if (from > end) break;

			int s = start, e = end;
			if (from > s) s = from;
			if (to < e) e = to;

			modify.emplace(std::make_pair(s, e), frag);
		}
		QTextCursor tempCursor(block);
		for (auto& [k, frag] : modify) {
			auto format = frag.charFormat();
			cb(format);

			tempCursor.setPosition(k.first);
			tempCursor.setPosition(k.second, QTextCursor::KeepAnchor);
			tempCursor.mergeCharFormat(format);
		}
	}

	cursor.endEditBlock();
}

void MemoDialog::saveInThread()
{
	//do
	//{
	//	///每4s中检测一次文档是否被修改
	//	std::unique_lock<std::mutex> lck(mtx_);
	//	cv_.wait_for(lck, std::chrono::milliseconds(200), [&] {return !keep_ || save_right_; });

	//	save();
	//} while (keep_);
}

void MemoDialog::save()
{
	//auto doc = ui.textEdit->document();
	//if (doc->isModified()) {
	//	doc->setModified(false);

	//	auto html = doc->toHtml();
	//	auto content = html.toLocal8Bit().toBase64();
	//	int len = content.length();

	//	memo_->SetHtml(html);
	//	auto prop = memo_->PropertyToStream();
	//	auto snap = memo_->Snapshot().toLocal8Bit().toBase64();
	//	auto time = QDateTime::currentDateTime().toString();
	//	memo_->SetTime(time);

	//	std::unique_ptr<char> data(new char[len + prop.length() + snap.length() + 256] { 0 });
	//	int r = sprintf(data.get(), R"(update Memo set DATA="%s", PROP="%s", SNAPSHOT="%s", TIME="%s" where ID=%u;)",
	//		content.data(),
	//		prop.data(),
	//		snap.data(),
	//		time.toLocal8Bit().data(),
	//		memo_->Id());

	//	//ExecSQL(db_, data.get());
	//	db_->Query(data.get());

	//	emit MemoUpdate(this);
	//}

	//save_right_ = false;
}