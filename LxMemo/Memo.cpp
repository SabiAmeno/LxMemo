#include "Memo.h"
#include <random>
#include <QIODevice>
#include <QFontMetrics>
#include <QDataStream>
#include <QTextDocument>
#include <QPainterPath>
#include <QTextDocumentFragment>

Memo::Memo()
    : Meta()
{
}

Memo::Memo(uint32_t id)
    : Meta(id)
{
}

static QFont G_YAHEI_FONT("Microsoft YaHei UI", 6);
static int YaHeiFontHeight()
{
    static QFontMetrics fmtx(G_YAHEI_FONT);

    return fmtx.height();
}

void Memo::paint(QPainter& painter, const QRect& rect) const
{
    QRect dst = rect;
    //dst.adjust(0, 0, -30, 0);
    //dst.setLeft(rect.left() + 25);

    painter.save();

    painter.setFont(G_YAHEI_FONT);

    painter.drawPixmap(0, 0, QPixmap(":/LxMemo/icons/note.png").scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //painter.drawPixmap(rect.topLeft(), QPixmap(":/LxMemo/icons/memo_file.png").scaled(16, 16));
    //painter.drawPixmap(rect.topRight() - QPoint(25, -5), QPixmap(":/LxMemo/icons/other.png"));
    
    QPainterPath path;
    path.addEllipse(rect.topLeft() + QPoint(6, 12), 4, 4);
    painter.fillPath(path, color_);

    //if (snapshot_.isEmpty()) {
        //painter.setPen(Qt::darkGray);
        //QString tips = "MEMO";
    QPixmap icon;
    switch (memo_type_)
    {
    case kMemoText:
        icon.load(":/LxMemo/icons/text.png");
        break;
    case kMemoHtml:
        icon.load(":/LxMemo/icons/html.png");
        break;
    case kMemoMarkdown:
        icon.load(":/LxMemo/icons/markdown.png");
        break;
    }
    icon = icon.scaled(32, 32);
    painter.drawPixmap(dst.center()-QPoint(icon.width() / 2, icon.height() / 2), icon);
    /*}
    else {
        int lineHight = YaHeiFontHeight();

        for (int i = 0; i < snapshot_.size(); i++) {
            int dx = dst.left() + 8,
                dy = dst.top() + i * lineHight + 18;

            painter.drawText(QRectF(dx, dy, dst.width(), lineHight), Qt::AlignLeft, snapshot_[i]);
        }
    }
    if (!time_.isEmpty()) {
        QFont font = painter.font();
        font.setPointSizeF(4);
        painter.setFont(font);
        painter.setPen(Qt::darkGreen);
        painter.drawText(dst.left() + 8, dst.bottom() - 6, time_);
    }*/

    painter.restore();
}

size_t Memo::type()
{
    return typeid(Memo).hash_code();
}

bool Memo::IsEmpty()
{
    return raw_.isEmpty();
}

//void Memo::SetHtml(const QString& html)
//{
//    html_ = html;
//    auto doc = QTextDocumentFragment::fromHtml(html_);
//    auto raw = doc.toPlainText();
//
//    snapshot_.clear();
//
//    int count = 0;
//    int ind = 0;
//
//    do
//    {
//        ind = raw.indexOf('\n', 0);
//        if (ind == -1) {
//            if (!raw.isEmpty())
//                snapshot_ << raw;
//
//            break;
//        }
//
//        count++;
//        snapshot_ << (raw.left(ind));
//        raw = raw.right(raw.length() - ind - 1);
//    } while (count < 5 && ind != -1);
//}

void Memo::SetRawData(const QString& raw)
{
    raw_ = raw;
}

QString Memo::GetRawData()
{
    return raw_;
}

//QString Memo::GetMarkdown() const
//{
//    auto doc = QTextDocumentFragment::fromHtml(html_);
//
//    return doc.toPlainText();
//}

void Memo::SetMemoType(MemoType type)
{
    memo_type_ = type;
}

MemoType Memo::GetMemoType()
{
    return memo_type_;
}

void Memo::SetSnapshot(const QString& snapshot)
{
    if (!snapshot.isEmpty()) {
        snapshot_ = snapshot.split('\n');
    }
}
