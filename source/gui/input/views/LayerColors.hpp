#pragma once
#include "bow/input/Layers.hpp"
#include <QtWidgets>
#include <random>

static QColor getLayerColor(const Layer& layer)
{
    // https://stackoverflow.com/a/19728404/4692009
    std::mt19937 rng(layer.rho*layer.E);
    std::uniform_int_distribution<int> uni(0, 255);

    return QColor(uni(rng), uni(rng), uni(rng));
}

static QPixmap getLayerPixmap(const Layer& layer)
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
