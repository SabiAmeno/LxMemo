#ifndef LUCENCYDIALOG_H
#define LUCENCYDIALOG_H

#include <QMap>
#include <QWidget>
#include <QToolButton>

namespace Ui {
    class LucencyDialog;
}

enum WindowAttribute : uchar
{
    WA_MINIMUM_WINDOW = 0x1,
    WA_MAXIMUM_WINDOW = 0x2,
    WA_CLOSE_WINDOW = 0x4
};

class LucencyDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LucencyDialog(QWidget* parent = nullptr);
    ~LucencyDialog();

    void SetOpacity(qreal op);
    void SetWidget(QWidget* w);
    void SetFixedEnable(bool en);
    void SetFlags(Qt::WindowFlags flags);
    void SetToolBarVisible(bool visible);
    void SetPureStyle(const QString& color);
    void SetTitle(const QString& title);
    void SetAttribute(uchar attr);
    void SetIcon(const QString& icon, bool visible = true);
    QToolButton* AddAction(const QIcon& icon, const QString& tip = "");
    void SetShadeWidth(int w);

    template<typename W>
    W Widget()
    {
        return qobject_cast<W>(widget);
    }

signals:
    void windowFixed(bool);
    void windowClosed();
protected:
    virtual bool eventFilter(QObject* w, QEvent* e) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    virtual void showEvent(QShowEvent* event) override;
    virtual void changeEvent(QEvent* event) override;
private slots:
    void on_ButtonMin_clicked();
    void on_ButtonMax_clicked();
    void on_ButtonClose_clicked();
    void on_ButtonFixed_clicked();

private:
    void initStyle();
private:
    Ui::LucencyDialog* ui;
    int shadeWidth = 10;

    QPoint pStart;
    bool   isMove{ false };

    ///窗体是否固定
    bool isFixed{ false };
    ///启用固定
    bool enFixed{ true };

    QWidget* widget{ nullptr };

    uchar           attrs{ 7 };
    Qt::WindowFlags flags;
};

#endif // LUCENCYDIALOG_H
