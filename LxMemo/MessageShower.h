#pragma once

#include <QTimer>
#include <QWidget>

class MessageShower : public QWidget
{
    Q_OBJECT

public:
    MessageShower(QWidget* parent);
    ~MessageShower();

    void setText(const QString& msg);

    static void Start(const QString& msg, QWidget* parent = 0);
protected:
    virtual void paintEvent(QPaintEvent* e) override;
private slots:
    void onTimeout();
private:
    QTimer _timer;
    QString _msg;
};
