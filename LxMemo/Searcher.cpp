#include "Searcher.h"
#include <QHBoxLayout>
#include <QTextDocument>
#include <QTextBlock>

Searcher::Searcher(QTextDocument* doc, QWidget* parent)
    : QWidget(parent), _doc(doc)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 0, 4);
    layout->setSpacing(0);

    _edit = new QLineEdit(this);
    _edit->setFixedHeight(28);
    _searchButton = new QToolButton(this);
    _searchButton->setIcon(QIcon(":/LxMemo/icons/search.png"));
    _searchButton->setFixedSize(28, 28);
    _searchButton->setAutoRaise(true);

    _closeButton = new QToolButton(this);
    _closeButton->setIcon(QIcon(":/LxMemo/icons/close.png"));
    _closeButton->setFixedSize(28, 28);
    _closeButton->setAutoRaise(true);

    layout->addWidget(_edit);
    layout->addWidget(_searchButton);
    layout->addWidget(_closeButton);
    setLayout(layout);

    _ligher = QSharedPointer<BlockHighlighter>(new BlockHighlighter(this));
    _ligher->setDocument(_doc);

    auto style = R"(QToolButton{
                        background-color: rgb(255,251,240);
                        border:0px groove gray;
                        border-radius:0px;
                        padding:0px 4px;
                        border-style: outset;
                        margin-right: 0px;
                    }
                    QToolButton:hover{
                        background-color:rgb(229, 241, 251);
                        color: black;}
                    QToolButton:pressed{
                        background-color:rgb(204, 228, 247);
                        border-style: inset;}
                    QLineEdit{border-radius: 0px;
                        border: 0px;
                        background-color: rgb(255,251,240);
                        margin-left: 4px;})";

    setStyleSheet(style);

    connect(_searchButton, &QToolButton::clicked, this, &Searcher::onSearch);
    connect(_closeButton, &QToolButton::clicked, this, &Searcher::onClose);
}

void Searcher::ResetSearcher()
{
    for (auto& sel : _sels) {
        QTextCharFormat format;
        format.clearBackground();
        sel.setCharFormat(format);
    }
    _sels.clear();
}

void Searcher::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Return)
    {
        onSearch();
    }

    QWidget::keyPressEvent(event);
}

void Searcher::hideEvent(QHideEvent* e)
{
    //if (QEvent::Hide == e->type())
    //    ResetSearcher();
    auto _ = QSharedPointer<BlockHighlighter>(new BlockHighlighter(this));
    _->setDocument(_doc);
    _ligher.swap(_);

    QWidget::hideEvent(e);
}

void Searcher::focusInEvent(QFocusEvent* event)
{
    _edit->setFocus();
}

void Searcher::onClose()
{
    hide();
}

void Searcher::onSearch()
{
    //ResetSearcher();

    auto searchText = _edit->text();
    if (searchText.isEmpty())
        return;

    _ligher->setRegexp(searchText);
    _ligher->rehighlight();
}

////////////////////
BlockHighlighter::BlockHighlighter(QObject* parent)
    : QSyntaxHighlighter(parent)
{
}

void BlockHighlighter::setRegexp(const QString& regexp)
{
    _regexp = regexp;
}

void BlockHighlighter::highlightBlock(const QString& text)
{
    if (_regexp.isEmpty())
        return;

    int ind = 0;

    do
    {
        ind = text.indexOf(_regexp, ind);
        if (ind == -1)
            break;
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Bold);
        myClassFormat.setBackground(Qt::darkMagenta);

        setFormat(ind, _regexp.length(), "red");
        ind += _regexp.length();
    } while (true);
}