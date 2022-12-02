#pragma once

#include "Meta.h"

class Folder : public Meta
{
public:
    Folder();
    Folder(uint32_t id);

    void setName(const QString& name) { name_ = name; }
    QString name() { return name_; }

    virtual void paint(QPainter& painter, const QRect& rect) const override;
    virtual size_t type() override;
private:
    
private:
    QString name_;
};

using SharedFolder = QSharedPointer<Folder>;

Q_DECLARE_METATYPE(SharedFolder);