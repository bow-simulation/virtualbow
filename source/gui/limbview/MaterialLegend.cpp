#include "MaterialLegend.hpp"
#include "LayerColors.hpp"
#include <QLabel>
#include <QHBoxLayout>

MaterialEntry::MaterialEntry()
    : symbol(new QLabel()),
      label(new QLabel())
{
    symbol->setFixedWidth(18);
    symbol->setFixedHeight(18);
    symbol->setScaledContents(true);
    label->setStyleSheet("color: white; font: 16px;");

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignTop);
    hbox->setMargin(0);
    hbox->addWidget(symbol, 0);
    hbox->addSpacing(10);
    hbox->addWidget(label, 1);
    this->setLayout(hbox);
}

void MaterialEntry::setData(const Material& material) {
    symbol->setPixmap(getColorPixmap(QColor(QString::fromStdString(material.color)), 24));
    label->setText(QString::fromStdString(material.name));
}

MaterialLegend::MaterialLegend()
    : vbox(new QVBoxLayout())
{
    vbox->setSpacing(10);
    this->setLayout(vbox);
}

void MaterialLegend::setData(const std::vector<Material>& materials) {
    while(vbox->count() < materials.size()) {
        vbox->addWidget(new MaterialEntry());
    }

    while(vbox->count() > materials.size()) {
        QWidget* widget = vbox->takeAt(0)->widget();
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    for(int i = 0; i < vbox->count(); ++i) {
        MaterialEntry* entry = dynamic_cast<MaterialEntry*>(vbox->itemAt(i)->widget());
        entry->setData(materials[i]);
    }

    vbox->update();
}
