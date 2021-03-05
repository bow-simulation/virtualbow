#include "LayerColors.hpp"
#include <random>

#include <boost/functional/hash.hpp>
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

#include <iostream>
QColor getLayerColor(double rho, double E)
{
    // Random distributions for HSV color components
    std::uniform_int_distribution<int> h(0, 75);
    std::uniform_int_distribution<int> s(150, 255);
    std::uniform_int_distribution<int> v(225, 255);

    // Create a seed from the material properties
    uint_fast64_t seed = 0;
    boost::hash_combine(seed, rho);
    boost::hash_combine(seed, E);

    // Generate random color from seed and distributions
    std::mt19937_64 rng(seed);
    return QColor::fromHsv(h(rng), s(rng), v(rng));
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
