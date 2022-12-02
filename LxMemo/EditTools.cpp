#include "EditTools.h"
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>

EditTools::EditTools(QTextEdit* edit, QWidget* parent)
    : QWidget(parent), _edit(edit)
{
    ui.setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    //setWindowOpacity(0.9);
    setFocusPolicy(Qt::NoFocus);

    ui.font_color_btn->SetIcon(":/LxMemo/icons/font.png");
    ui.font_bkcolor_btn->SetIcon(":/LxMemo/icons/brush.png");
    ui.font_bkcolor_btn->SetColorFace(new ColorGrid(this));
    ui.font_color_btn->SetColorFace(new ColorGrid(this));

    connect(ui.font_bold_btn, &QToolButton::clicked, this, &EditTools::changeBold);
    connect(ui.font_italic_btn, &QToolButton::clicked, this, &EditTools::changeItalic);
    connect(ui.font_strikeout_btn, &QToolButton::clicked, this, &EditTools::changeStrikeout);
    connect(ui.font_underline_btn, &QToolButton::clicked, this, &EditTools::changeUnderline);
    connect(ui.font_color_btn, &ColorButton::colorChnaged, this, &EditTools::changeFontColor);
    connect(ui.font_bkcolor_btn, &ColorButton::colorChnaged, this, &EditTools::changeBackgroundColor);
    connect(ui.font_up_btn, &QToolButton::clicked, this, &EditTools::changeFontUp);
    connect(ui.font_down_btn, &QToolButton::clicked, this, &EditTools::changeFontDown);
    //connect(ui.comboBox, &QComboBox::currentTextChanged, this, &EditTools::changeFontSize);
}

EditTools::~EditTools()
{}

void EditTools::UpdateTool(const QTextCharFormat& format)
{
    ui.comboBox->clear();

    auto font = format.font();
    QFontDatabase db;
    auto sizes = db.pointSizes(font.family());

    disconnect(ui.comboBox, &QComboBox::currentTextChanged, this, &EditTools::changeFontSize);
    //connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &EditTools::onFontChanged);
    for (auto s : sizes)
        ui.comboBox->addItem(QString::number(s));

    ui.font_up_btn->setChecked(format.verticalAlignment() == QTextCharFormat::AlignSuperScript);
    ui.font_down_btn->setChecked(format.verticalAlignment() == QTextCharFormat::AlignSubScript);
    ui.font_bold_btn->setChecked(font.bold());
    ui.font_italic_btn->setChecked(font.italic());
    ui.font_underline_btn->setChecked(font.underline());
    ui.font_strikeout_btn->setChecked(font.strikeOut());
    ui.comboBox->setCurrentText(QString::number(font.pointSize()));
    ui.fontComboBox->setCurrentFont(font);

    ui.font_color_btn->SetColor(format.foreground().color());
    ui.font_bkcolor_btn->SetColor(format.background().color());

    connect(ui.comboBox, &QComboBox::currentTextChanged, this, &EditTools::changeFontSize);
    connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &EditTools::onFontChanged);
}

void EditTools::onFontColorChanged()
{
    //emit changeFontColor(ui.font_color_btn->Color());
}

void EditTools::onFontBkColorChanged()
{
    //emit changeBackgroundColor(ui.font_bkcolor_btn->Color());
}

void EditTools::onFontChanged(const QFont& font)
{
    auto f = font;
    f.setPointSizeF(ui.comboBox->currentText().toInt());
    emit changeFont(f);
}

void EditTools::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
}

void EditTools::hideEvent(QHideEvent* event)
{
    disconnect(ui.comboBox, &QComboBox::currentTextChanged, this, &EditTools::changeFontSize);
    connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &EditTools::onFontChanged);

    QWidget::hideEvent(event);
}