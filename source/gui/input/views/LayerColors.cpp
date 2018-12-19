#include "LayerColors.hpp"
#include <random>

#include <QtCore>

// https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
// https://stackoverflow.com/a/19728404/4692009
QColor getLayerColor(const Layer& layer)
{
    std::mt19937 rng(unsigned(layer.rho)*unsigned(layer.E));
    std::uniform_real_distribution<qreal> uni(0.0, 1.0);
    return QColor::fromHsvF(uni(rng), 0.8, 1.0);
}

QPixmap getLayerPixmap(const Layer& layer)
{
    const int size = 48;
    const int line_width = 2;
    const qreal rounding = 0.2;

    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen({Qt::black, line_width});
    painter.setBrush(getLayerColor(layer));
    QPainterPath path;
    path.addRoundedRect({line_width/2, line_width/2, size - line_width, size - line_width},
                         rounding*size, rounding*size);
    painter.drawPath(path);

    return pixmap;
}
