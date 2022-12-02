#include "SizeSelector.h"
#include <QImageReader>

SizeSelector::SizeSelector(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    auto style = R"(
    QToolButton:pressed,
                 QToolButton:hover {
                     background-color:white;
                     border:0px groove white;
                 }
    QToolButton {border:0px groove gray;}
    QLineEdit {
        border: 1px solid gray;
        border-radius: 5px;
        padding: 0 8px;
        background: #ffffff;
        height: 20px;
        selection-background-color: darkgray;
    }
    )";

    setStyleSheet(style);
}

SizeSelector::~SizeSelector()
{}

void SizeSelector::Init(const QString& img)
{
    _imgFile = img;

    QImageReader reader(img);
    reader.setAutoDetectImageFormat(true);
    if (!reader.canRead()) {
        auto err = reader.errorString();
        return;
    }
    if (!reader.read(&_img))
        return;

    ui.edit_w->setText(QString::number(_img.width()));
    ui.edit_h->setText(QString::number(_img.height()));
}

void SizeSelector::on_toolButton_clicked()
{
    QTextImageFormat format;
    format.setWidth(ui.edit_w->text().toInt());
    format.setHeight(ui.edit_h->text().toInt());
    format.setName(_imgFile);

    emit SelectFinished(format);
}