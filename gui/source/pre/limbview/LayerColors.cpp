#include "LayerColors.hpp"
#include "solver/model/input/InputData.hpp"

#include <QPainterPath>
#include <QPainter>
#include <QPixmap>

QColor getLayerColor(const Layer& layer, const std::vector<Material>& materials) {
    if(layer.material < 0 || layer.material >= materials.size()) {
        return QColor();
    }

    auto& material = materials[layer.material];
    return QColor(QString::fromStdString(material.color));
}

QPixmap getLayerPixmap(const Layer& layer, const std::vector<Material>& materials, int size) {
    QColor color = getLayerColor(layer, materials);
    return getColorPixmap(color, size);
}

// Paints a rectangle of given color and size with a 1px dark grey border around it
QPixmap getColorPixmap(const QColor& color, int size) {
    QPixmap pixmap(size, size);
    pixmap.fill(color);

    QPainterPath path;
    path.addRect(0, 0, size-1, size-1);

    QPainter painter(&pixmap);
    painter.setPen(QColor::fromRgb(38, 38, 38));
    painter.drawPath(path);

    return pixmap;
}
