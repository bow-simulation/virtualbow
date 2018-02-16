#include "LayerColors.hpp"
#include <random>

// https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
// https://stackoverflow.com/a/19728404/4692009
QColor getLayerColor(const Layer& layer)
{
    std::mt19937 rng(layer.rho*layer.E);
    std::uniform_real_distribution<double> uni(0.0, 1.0);

    return QColor::fromHsvF(uni(rng), 0.8, 1.0);
}

/*
std::vector<QColor> getLayerColors(const Layers& layers)
{
    double h_min = 0.0;
    double h_max = 0.2;

    double s_min = 0.5;
    double s_max = 1.0;

    auto cmp_rho = [](const Layer& lhs, const Layer& rhs){ return lhs.rho < rhs.rho; };
    auto cmp_E = [](const Layer& lhs, const Layer& rhs){ return lhs.E < rhs.E; };

    double rho_min = std::min_element(layers.begin(), layers.end(), cmp_rho)->rho;
    double rho_max = std::max_element(layers.begin(), layers.end(), cmp_rho)->rho;
    double E_min = std::min_element(layers.begin(), layers.end(), cmp_E)->E;
    double E_max = std::max_element(layers.begin(), layers.end(), cmp_E)->E;

    std::vector<QColor> colors;
    for(auto& layer: layers)
    {
        double h = (E_min < E_max)
                 ?  h_min + (layer.E - E_min)/(E_max - E_min)*(h_max - h_min)
                 :  0.5*(h_min + h_max);

        double s = (rho_min < rho_max)
                 ?  s_min + (layer.rho - rho_min)/(rho_max - rho_min)*(s_max - s_min)
                 :  0.5*(s_min + s_max);

        qInfo() << "h = " << h;
        qInfo() << "s = " << s;

        colors.push_back(QColor::fromHsvF(h, s, 1.0));
    }

    return colors;
}
*/

QPixmap getLayerPixmap(const Layer& layer)
{
    // https://stackoverflow.com/a/29196812/4692009
    // https://stackoverflow.com/a/37213313/4692009

    const double size = 48.0;
    const QColor color = getLayerColor(layer);

    QPixmap pixmap(size, size);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(QRectF{0.0, 0.0, size, size}, color);

    //QPainterPath path;
    //path.addRoundedRect(QRectF(0, 0, size, size), 0.3*size, 0.3*size);

    //painter.setPen(Qt::NoPen);
    //painter.fillPath(path, color);
    //painter.drawPath(path);

    return pixmap;
}
