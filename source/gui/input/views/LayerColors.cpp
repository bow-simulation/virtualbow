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
	const int offset = 6;

    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainterPath path;
    path.addRect(offset, offset, size - 2*offset, size - 2*offset);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
	painter.fillPath(path, getLayerColor(layer));
    painter.drawPath(path);

    return pixmap;
}
