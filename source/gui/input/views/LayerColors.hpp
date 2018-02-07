#pragma once
#include "bow/input/Layers.hpp"
#include <QtCore>
#include <random>

QColor getLayerColor(const Layer& layer)
{
    // https://stackoverflow.com/a/19728404/4692009
    std::mt19937 rng(layer.rho*layer.E);
    std::uniform_int_distribution<int> uni(0, 255);

    return QColor(uni(rng), uni(rng), uni(rng));
}
