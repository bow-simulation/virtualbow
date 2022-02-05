#include "LayerColors.hpp"
#include "solver/model/input/InputData.hpp"
#include "solver/model/LimbProperties.hpp"

#include <boost/functional/hash.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <random>

#include <QPainterPath>
#include <QPainter>
#include <QPixmap>

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
    using boost::random::uniform_int_distribution;
    using boost::random::mt19937_64;
    using boost::hash_combine;

    // Random distributions for HSV color components
    uniform_int_distribution<int> h(0, 359);
    uniform_int_distribution<int> s(150, 200);
    uniform_int_distribution<int> v(150, 200);

    // Create a seed from the material properties
    size_t seed = 0;
    hash_combine(seed, rho);
    hash_combine(seed, E);

    // Generate random color from seed and distributions
    mt19937_64 rng(seed);
    return QColor::fromHsv(h(rng), s(rng), v(rng));
}

QPixmap getLayerPixmap(const Layer& layer, int size)
{
    return getColorPixmap(getLayerColor(layer), size);
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
