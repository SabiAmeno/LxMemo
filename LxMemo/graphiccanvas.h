#ifndef GRAPHICCANVAS_H
#define GRAPHICCANVAS_H

#include <QWidget>
#include "graphgoup.h"
#include "GraphNote.h"
#include "TextStyleWidget.h"
#include "ui_graphiccanvas.h"

enum SizeDirect {
	kDirectionNone,
	kDirectionLeft,
	kDirectionTop,
	kDirectionRight,
	kDirectionBottom,
	kDirectionTopLeft,
	kDirectionTopRight,
	kDirectionBottomRight,
	kDirectionBottomLeft,
};

class GraphicCanvas : public QWidget
{
	Q_OBJECT

public:
	explicit GraphicCanvas(QWidget* parent = nullptr);
	~GraphicCanvas();

	QByteArray SerializeTo();
	void DeserializeFrom(QByteArray&& data);
public slots:
	void onWordAdd();
	void onPictureAdd();
	void onGraphAdd();
signals:
	void CanvasChanged();
	void GraphClicked(Graph*);
private slots:

	void onDeleteGraph();
	void onMoveUp();
	void onMoveDown();
	void onMoveTop();
	void onMoveBottom();

	void onAddText();
	void onAddPicture();
protected:
	virtual void paintEvent(QPaintEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void wheelEvent(QWheelEvent* e) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* e) override;
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void keyReleaseEvent(QKeyEvent* e) override;
	virtual void closeEvent(QCloseEvent* e) override;
private:
	void changeCursor(const QRect& geo, const QPoint& p);
private:
	Ui::GraphicCanvas ui;

	QPoint move_start_{};
	GraphGroup* group_{ nullptr };
	Graph* selected_graph_{ nullptr };
	SizeDirect cursor_direction_{ kDirectionNone };
};

#endif // GRAPHICCANVAS_H
