#include "NoFrameWidget.h"
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

NoFrameWidget::~NoFrameWidget()
{}

NoFrameWidget::NoFrameWidget(QWidget* parent) :
	QWidget(parent)
{
	ui.setupUi(this);
	setAutoFillBackground(true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

	//
	//setWindowOpacity(0.5);

	//initStyle();
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

void NoFrameWidget::SetOpacity(qreal op)
{
	setWindowOpacity(op);
}

void NoFrameWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	QPainterPath path;
	path.addRoundRect(QRect(shadeWidth, shadeWidth, this->width() - 2 * shadeWidth, this->height() - 2 * shadeWidth), 4);
	//painter.fillRect(QRect(shadeWidth, shadeWidth, this->width() - 2 * shadeWidth, this->height() - 2 * shadeWidth), QBrush(Qt::white));
	painter.fillPath(path, QBrush(Qt::white));

	QColor color(38, 78, 119, 100);
	for (int i = 1; i < shadeWidth; i++)
	{
		color.setAlpha(60 - sqrt(i) * 20);
		painter.setPen(color);
		// 方角阴影边框;
		//painter.drawRect(shadeWidth - i, shadeWidth - i, this->width() - (shadeWidth - i) * 2, this->height() - (shadeWidth - i) * 2);
		// 圆角阴影边框;
		painter.drawRoundedRect(shadeWidth - i, shadeWidth - i, this->width() - (shadeWidth - i) * 2, this->height() - (shadeWidth - i) * 2, 4, 4);
	}

	return QWidget::paintEvent(event);
}

bool NoFrameWidget::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	int m_nBorder = 10;
	Q_UNUSED(eventType);
	//if (!isFixed && ui.widget->isVisible()) {
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
	//}
	return QWidget::nativeEvent(eventType, message, result);
}

void NoFrameWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
}

void NoFrameWidget::SetPureStyle(const QString& color)
{
	//setStyleSheet(0);
	auto style = tr("background-color: %1;").arg(color);
	setStyleSheet(style);
	repaint();
	//ui->widget->setStyleSheet(tr("background-color: %1;").arg(color));
	//ui->widget->setStyleSheet("background-color:white;");
	//setStyleSheet(tr("QFrame#frame{border: 1px solid %1;background-color: %2;}").arg(color, color));
}