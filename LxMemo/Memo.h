#pragma once

#include <QString>
#include <QPainter>
#include "Meta.h"

class Memo : public Meta
{
public:
    Memo();
    Memo(uint32_t id);

    virtual void paint(QPainter& painter, const QRect& rect) const override;
    virtual size_t type() override;

    void SetHtml(const QString& html);
    QString GetHtml() const { return html_; }
    QString GetMarkdown() const;

    void SetSnapshot(const QString& snapshot);
    QString Snapshot() { return snapshot_.join('\n'); }
private:

    QStringList snapshot_;
    QString     html_;
};

using SharedMemo = QSharedPointer<Memo>;

Q_DECLARE_METATYPE(SharedMemo);