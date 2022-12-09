#pragma once

#include <QString>
#include <QPainter>
#include "Meta.h"

enum MemoType
{
    kMemoText,
    kMemoHtml,
    kMemoMarkdown,
};

class Memo : public Meta
{
public:
    Memo();
    Memo(uint32_t id);

    virtual void paint(QPainter& painter, const QRect& rect) const override;
    virtual size_t type() override;

    bool IsEmpty();
    //void SetHtml(const QString& html);
    //QString GetHtml() const { return raw_; }
    void SetRawData(const QString& raw);
    QString GetRawData();

    //QString GetMarkdown() const;
    void SetMemoType(MemoType type);
    MemoType GetMemoType();

    void SetSnapshot(const QString& snapshot);
    QString Snapshot() { return snapshot_.join('\n'); }
private:
    MemoType memo_type_{ MemoType::kMemoText };
    QStringList snapshot_;
    //QString     html_;
    QString raw_;
};

using SharedMemo = QSharedPointer<Memo>;

Q_DECLARE_METATYPE(SharedMemo);