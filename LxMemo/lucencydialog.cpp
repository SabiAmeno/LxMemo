#include "lucencydialog.h"
#include "ui_lucencydialog.h"
#include <QMouseEvent>
#include <Windows.h>
#include <windowsx.h>
#include <QAction>
#include <QSystemTrayIcon>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QPainter>
#include "db.h"

LucencyDialog::LucencyDialog(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::LucencyDialog)
{
	ui->setupUi(this);
	setAutoFillBackground(true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	//
	//setWindowOpacity(0.5);

	ui->widget->installEventFilter(this);

	initStyle();
#if 0 ///此处使用阴影边框，会影响窗口stylesheet的动态设置
	//设置阴影边框
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(0, 0);

	//阴影颜色
	shadow_effect->setColor(QColor(38, 78, 119, 100));

	//阴影半径
	shadow_effect->setBlurRadius(16);

	setGraphicsEffect(shadow_effect);
#endif
	setAttribute(Qt::WA_TranslucentBackground);
}

LucencyDialog::~LucencyDialog()
{
	delete ui;
}

void LucencyDialog::SetOpacity(qreal op)
{
	setWindowOpacity(op);
}

void LucencyDialog::SetWidget(QWidget* w)
{
	if (!w)
		return;
	if (widget) {
		widget->removeEventFilter(this);
		ui->frame->layout()->removeWidget(widget);
		delete widget; widget = nullptr;
	}
	auto title = w->windowTitle();
	if (!title.isEmpty())
		SetTitle(title);

	widget = w;
	widget->installEventFilter(this);
	ui->frame->layout()->addWidget(w);

	ui->frame->adjustSize();
	adjustSize();
}

void LucencyDialog::SetFixedEnable(bool en)
{
	enFixed = en;

	ui->ButtonFixed->setVisible(en);
}

void LucencyDialog::SetFlags(Qt::WindowFlags flags)
{
	auto f = windowFlags();
	f |= flags;
	setWindowFlags(f);
}

void LucencyDialog::SetToolBarVisible(bool visible)
{
	ui->widget->setVisible(visible);
}

void LucencyDialog::SetPureStyle(const QString& color)
{
	//setStyleSheet(0);
	auto style = tr("background-color: %1;").arg(color);
	setStyleSheet(style);
	if(widget)
		widget->setStyleSheet(style);
	repaint();
	//ui->widget->setStyleSheet(tr("background-color: %1;").arg(color));
	//ui->widget->setStyleSheet("background-color:white;");
	//setStyleSheet(tr("QFrame#frame{border: 1px solid %1;background-color: %2;}").arg(color, color));
}

void LucencyDialog::SetTitle(const QString& title)
{
	ui->title->setFont(QFont("Microsoft YaHei UI", 10, 2));
	ui->title->setText(title);
}

#define MASK(a) (0x7 & (a))

void LucencyDialog::SetAttribute(uchar attr)
{
	attrs &= MASK(attr);

	bool vi = attrs & WA_MINIMUM_WINDOW;
	ui->ButtonMin->setVisible(vi);

	vi = attrs & WA_MAXIMUM_WINDOW;
	ui->ButtonMax->setVisible(vi);

	vi = attrs & WA_CLOSE_WINDOW;
	ui->ButtonClose->setVisible(vi);
}

void LucencyDialog::SetIcon(const QString& icon, bool visible)
{
	ui->icon->setVisible(visible);
	ui->icon->setPixmap(QPixmap(icon));
}

QToolButton* LucencyDialog::AddAction(const QIcon& icon, const QString& tip)
{
	QToolButton* button = new QToolButton(ui->widget);
	button->setIcon(icon);
	button->setAutoRaise(true);
	button->setFixedSize(28, 28);
	button->setToolTip(tip);
	button->setIconSize(QSize(26, 26));
	ui->actions_layout->addWidget(button);

	auto style = R"( QToolButton:pressed {
                        background-color: white;
                        border:3px groove white;
                        }
                     QToolButton:hover {
                        background-color:white;
                        border:3px groove white;
                     }
					)";
	button->setStyleSheet(style);

	return button;
}

void LucencyDialog::SetShadeWidth(int w)
{
	shadeWidth = w;
	setContentsMargins(w, w, w, w);
}

bool LucencyDialog::eventFilter(QObject* w, QEvent* e)
{
	if (w == ui->widget)
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
			if (isFixed || !isMove)
				break;
			auto me = (QMouseEvent*)e;
			auto diff = me->pos() - pStart;
			move(pos() + diff);
			break;
		}
		case QEvent::MouseButtonDblClick:
		{
			if (attrs & WA_MAXIMUM_WINDOW)
				on_ButtonMax_clicked();
			break;
		}
		default:
			break;
		}
	}

	return QWidget::eventFilter(w, e);
}

void LucencyDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	QPainterPath path;
	path.addRoundRect(QRect(shadeWidth, shadeWidth, this->width() - 2 * shadeWidth, this->height() - 2 * shadeWidth), 4);
	//painter.fillRect(QRect(shadeWidth, shadeWidth, this->width() - 2 * shadeWidth, this->height() - 2 * shadeWidth), QBrush(Qt::white));
	painter.fillPath(path, QBrush(Qt::white));

	QColor color(38, 78, 119, 100);
	for (int i = 0; i < shadeWidth; i++)
	{
		color.setAlpha(120 - sqrt(i) * 40);
		painter.setPen(color);
		// 方角阴影边框;
	    painter.drawRect(shadeWidth - i, shadeWidth - i, this->width() - (shadeWidth - i) * 2, this->height() - (shadeWidth - i) * 2);
		// 圆角阴影边框;
		//painter.drawRoundedRect(shadeWidth - i, shadeWidth - i, this->width() - (shadeWidth - i) * 2, this->height() - (shadeWidth - i) * 2, 4, 4);
	}

	return QWidget::paintEvent(event);
}

bool LucencyDialog::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	int m_nBorder = 10;
	Q_UNUSED(eventType);
	if (!isFixed && ui->widget->isVisible()) {
		MSG* param = static_cast<MSG*>(message);

		switch (param->message)
		{
		case WM_NCHITTEST:
		{
			int nX = GET_X_LPARAM(param->lParam) - this->geometry().x();
			int nY = GET_Y_LPARAM(param->lParam) - this->geometry().y();

			//// if mouse in the child Qwidget of mainfrme, we should not handle it
			//if (childAt(nX, nY) != NULL)
			//  return QWidget::nativeEvent(eventType, message, result);

			*result = HTCAPTION;

			// if mouse in the border of mainframe,we zoom mainframe
			if ((nX > 0) && (nX < m_nBorder))
				*result = HTLEFT;

			if ((nX > this->width() - m_nBorder) && (nX < this->width()))
				*result = HTRIGHT;

			if ((nY > 0) && (nY < m_nBorder))
				*result = HTTOP;

			if ((nY > this->height() - m_nBorder) && (nY < this->height()))
				*result = HTBOTTOM;

			if ((nX > 0) && (nX < m_nBorder) && (nY > 0)
				&& (nY < m_nBorder))
				*result = HTTOPLEFT;

			if ((nX > this->width() - m_nBorder) && (nX < this->width())
				&& (nY > 0) && (nY < m_nBorder))
				*result = HTTOPRIGHT;

			if ((nX > 0) && (nX < m_nBorder)
				&& (nY > this->height() - m_nBorder) && (nY < this->height()))
				*result = HTBOTTOMLEFT;

			if ((nX > this->width() - m_nBorder) && (nX < this->width())
				&& (nY > this->height() - m_nBorder) && (nY < this->height()))
				*result = HTBOTTOMRIGHT;

			if (*result == HTCAPTION)
			{
				return false;
			}
			return true;
		}
		}
	}
	return QWidget::nativeEvent(eventType, message, result);
}

void LucencyDialog::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
}

void LucencyDialog::on_ButtonMin_clicked()
{
	showMinimized();
}

void LucencyDialog::on_ButtonMax_clicked()
{
	if (isMaximized())
		showNormal();
	else
		showMaximized();
}

void LucencyDialog::on_ButtonClose_clicked()
{
	//if (QSystemTrayIcon::isSystemTrayAvailable()) {
	//    hide();
	//}
	//else {
	//close();
	hide();
	emit windowClosed();
	//}
}

void LucencyDialog::on_ButtonFixed_clicked()
{
	isFixed = !isFixed;

	if (isFixed) {
		ui->ButtonFixed->setIcon(QIcon(":/LxMemo/icons/fixed.png"));
		//setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
		::SetWindowPos((HWND)winId(), HWND_TOPMOST, pos().x(), pos().y(), width(), height(), SWP_SHOWWINDOW);
	}
	else {
		ui->ButtonFixed->setIcon(QIcon(":/LxMemo/icons/unfixed.png"));
		//setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
		::SetWindowPos((HWND)winId(), HWND_NOTOPMOST, pos().x(), pos().y(), width(), height(), SWP_SHOWWINDOW);
	}

	//show();
	//emit windowFixed(isFixed);
}

void LucencyDialog::initStyle()
{
	auto style = R"(
                 QToolButton#ButtonClose:pressed,
                 QToolButton#ButtonClose:hover {
                     background-color:rgb(200,0,0);
                     border:0px groove rgb(200,0,0);
                 }
				 QToolButton {border-radius:6px;}
                )";
	ui->ButtonClose->setStyleSheet(style);

	style = R"(  QToolButton:pressed,
                 QToolButton:hover {
                     background-color:white;
                     border:0px groove white;
                 }
				 QToolButton {border-radius:6px;})";
	ui->ButtonMax->setStyleSheet(style);
	ui->ButtonMin->setStyleSheet(style);
	ui->ButtonFixed->setStyleSheet(style);
}

void LucencyDialog::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::WindowStateChange)
	{
		if (!isMinimized())
		{
			setAttribute(Qt::WA_Mapped);
		}
	}
	QWidget::changeEvent(event);
}