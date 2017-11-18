#include "DoubleView.hpp"

DoubleView::DoubleView(DocumentItem<double>& doc_item)
    : doc_item(doc_item)
{
    this->setValidator(new DoubleValidator(doc_item));

    QObject::connect(&doc_item, &DocumentNode::value_changed, this, &DoubleView::update_value);
    QObject::connect(&doc_item, &DocumentNode::error_changed, this, &DoubleView::update_error);

    update_value();
    update_error();
}

void DoubleView::update_value()
{
    if(!this->hasFocus())
        this->setText(QLocale::c().toString(doc_item, 'g'));
}

void DoubleView::update_error()
{
    if(doc_item.get_errors().size() == 0)
    {
        QPalette palette;
        palette.setColor(QPalette::Base, Qt::white);
        this->setPalette(palette);
        this->setToolTip("");
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Base, QColor(255, 102, 102));    // Magic number
        this->setPalette(palette);
        this->setToolTip(QString::fromStdString(this->doc_item.get_errors().front()));
    }
}

DoubleValidator::DoubleValidator(DocumentItem<double>& doc_item)
    : doc_item(doc_item)
{

}

// Set document value if input valid
QValidator::State DoubleValidator::validate(QString &input, int &pos) const
{
    State state = QDoubleValidator::validate(input, pos);
    if(state == QValidator::Acceptable)
    {
        doc_item = QLocale::c().toDouble(input);
    }

    return state;
}

// Reset to document value if input invalid after editing finished
void DoubleValidator::fixup(QString& input) const
{
    input = QLocale::c().toString(doc_item, 'g');
}
