#include "TitleBar.h"
#include <QToolButton>
#include <QMouseEvent>
/// <summary>
/// TITLE BAR
/// </summary>
/// <param name="parent"></param>

TitleBar::TitleBar(QWidget* parent)
	: QToolBar(parent)
{
	//ui.setupUi(this);
	setFixedHeight(24);

	icon_label_ = new QLabel();
	icon_label_->setFixedSize(22, 22);
	icon_label_->setPixmap(QPixmap(":/LxMemo/icons/memo_main.png").scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	addWidget(icon_label_);

	title_label_ = new QLabel();
	title_label_->setContentsMargins(2, 0, 0, 0);
	title_label_->setFont(QFont("Microsoft YaHei UI", 10));
	title_label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	addWidget(title_label_);

	auto close_action = new QAction(QIcon(":/LxMemo/icons/close.png"), "", this);
	auto max_action = new QAction(QIcon(":/LxMemo/icons/maximum.png"), "", this);
	auto min_action = new QAction(QIcon(":/LxMemo/icons/minimum.png"), "", this);
	close_action->setObjectName("close_action");
	min_action->setObjectName("min_action");
	max_action->setObjectName("max_action");

	addAction(min_action);
	addAction(max_action);
	addAction(close_action);


    initStyle();

	installEventFilter(this);

    connect(close_action, &QAction::triggered, this, &TitleBar::closeButtonClicked);
    connect(min_action, &QAction::triggered, this, &TitleBar::minimumButtonClicked);
	connect(max_action, &QAction::triggered, this, &TitleBar::maximumButtonClicked);
}

void TitleBar::SetTitle(const QString& title)
{
	title_label_->setText(title);
}

void TitleBar::SetIcon(const QString& icon)
{
	icon_label_->setPixmap(QPixmap(icon).scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void TitleBar::initStyle()
{
	auto style = R"(
			     QToolButton{
				    width: 20px;height:20px;
					border: 0px groove white;
					border-radius: 2px;
				 }
				 QToolButton:hover{
					background-color:white;
				 }
                )";

	setStyleSheet(style);
}

bool TitleBar::eventFilter(QObject* w, QEvent* e)
{
	auto type = e->type();
	switch (type) {
	case QEvent::MouseButtonPress:
	{
		auto me = (QMouseEvent*)e;
		if (me->button() == Qt::LeftButton) {
			pStart = me->pos();
			isMove = true;
		}
		break;
	}
	case QEvent::MouseButtonRelease:
	{
		isMove = false;
		break;
	}
	case QEvent::MouseMove:
	{
		if (!isMove)
			break;
		auto me = (QMouseEvent*)e;
		auto diff = me->pos() - pStart;
		//move(pos() + diff);
		//pStart = me->pos();
		emit positionChanged(diff);
		break;
	}
	//case QEvent::MouseButtonDblClick:
	//{
	//	if (attrs & WA_MAXIMUM_WINDOW)
	//		on_ButtonMax_clicked();
	//	break;
	//}
	default:
		break;
	}

	return QWidget::eventFilter(w, e);
}