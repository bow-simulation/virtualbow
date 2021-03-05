#include "LayerColors.hpp"
#include <random>

#include <QtCore>

// https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
// https://stackoverflow.com/a/19728404/4692009
QColor getLayerColor(const Layer& layer)
{
    return getLayerColor(layer.rho, layer.E);
}

QColor getLayerColor(const LayerProperties& layer)
{
    return getLayerColor(layer.rho, layer.E);
}

QColor getLayerColor(double rho, double E)
{
    // Random distributions for HSV color components
    std::uniform_int_distribution<int> h(0, 90);
    std::uniform_int_distribution<int> s(150, 255);
    std::uniform_int_distribution<int> v(200, 255);

    std::mt19937 rng((unsigned) (rho*E));    // Seed randum number generator with material properties
    return QColor::fromHsv(h(rng), s(rng), v(rng));    // Generate random color from distributions
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
