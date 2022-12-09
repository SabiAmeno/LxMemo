#include "TextStyleWidget.h"
#include "textgraph.h"
#include "graphicgraph.h"

TextStyleWidget::TextStyleWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.color_button->SetIcon(":/LxMemo/icons/brush.png");
    ui.bk_color_button->SetIcon(":/LxMemo/icons/brush.png");

    ui.color_button->SetColorFace(new ColorGrid(this));
    ui.bk_color_button->SetColorFace(new ColorGrid(this));

    auto sizes = QFontDatabase::standardSizes();
    for (auto sz : sizes)
        ui.comboBox->addItem(QString::number(sz));

    connect(ui.color_button, &ColorButton::colorChanged, this, &TextStyleWidget::onColorChanged);
    connect(ui.bk_color_button, &ColorButton::colorChanged, this, &TextStyleWidget::onBkColorChanged);
    connect(ui.comboBox, &QComboBox::currentTextChanged, this, &TextStyleWidget::onFontSizeChanged);
    connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &TextStyleWidget::onFontChanged);
}

TextStyleWidget::~TextStyleWidget()
{}

void TextStyleWidget::SetGraph(Graph * graph)
{
    graph_ = graph;

    init();
}

void TextStyleWidget::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    QWidget::paintEvent(e);
}

void TextStyleWidget::onColorChanged(const QColor& color)
{
    auto text_graph = graph_->converTo<TextGraph>();
    if (text_graph) {
        auto style = text_graph->GetTextStyle();
        style.color_ = color;
        text_graph->SetTextStyle(style);
    }
}

void TextStyleWidget::onBkColorChanged(const QColor& color)
{
    auto text_graph = graph_->converTo<TextGraph>();
    if (text_graph) {
        auto style = text_graph->GetTextStyle();
        style.bk_color_ = color;
        text_graph->SetTextStyle(style);
    }
}

void TextStyleWidget::onFontChanged(const QFont& font)
{
    auto text_graph = graph_->converTo<TextGraph>();
    if (text_graph) {
        auto style = text_graph->GetTextStyle();
        style.font_ = font;
        text_graph->SetTextStyle(style);
    }
}

void TextStyleWidget::onFontSizeChanged(const QString& sz)
{
    auto text_graph = graph_->converTo<TextGraph>();
    if (text_graph) {
        auto style = text_graph->GetTextStyle();
        style.font_.setPointSize(sz.toInt());
        text_graph->SetTextStyle(style);
    }
}

void TextStyleWidget::init()
{
    auto type = graph_->Type();

    switch (type)
    {
    case kGraphText:
    {
        auto text_graph = dynamic_cast<TextGraph*>(graph_);
        auto style = text_graph->GetTextStyle();
        ui.color_button->SetColor(style.color_);
        ui.bk_color_button->SetColor(style.bk_color_);
        ui.fontComboBox->setCurrentFont(style.font_);

        ui.comboBox->setCurrentText(QString::number(style.font_.pointSize()));
        break;
    }
    case kGraphPicture:
        break;
    case kGraphGroup:
        break;
    case kGraphNone:
        break;
    }
}
