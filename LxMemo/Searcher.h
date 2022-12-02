#pragma once
#include <QWidget.h>
#include <QLineEdit>
#include <QToolButton>
#include <QSyntaxHighlighter>
#include <QSharedPointer>

class BlockHighlighter;
class AnimationWidget : public QWidget
{
    Q_OBJECT
public:
    AnimationWidget(QWidget* parent = nullptr) {}
};

class Searcher :
    public QWidget
{
    Q_OBJECT
public:
    Searcher(QTextDocument* doc, QWidget* parent = nullptr);

    void ResetSearcher();
protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void hideEvent(QHideEvent* event) override;
    virtual void focusInEvent(QFocusEvent* event) override;
private slots:
    void onSearch();
    void onClose();
private:
    QLineEdit* _edit = nullptr;
    QToolButton* _searchButton = nullptr;
    QToolButton* _closeButton = nullptr;
    QTextDocument* _doc = nullptr;

    QSharedPointer<BlockHighlighter> _ligher = nullptr;
    QVector<QTextCursor> _sels;
};

class BlockHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    BlockHighlighter(QObject* parent = nullptr);

    void setRegexp(const QString& regexp);
protected:
    virtual void highlightBlock(const QString& text) override;

private:
    QString _regexp;
};