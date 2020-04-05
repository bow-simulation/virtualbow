#pragma once
#include "solver/model/input/Layers.hpp"
#include "solver/model/LimbProperties.hpp"
#include <QtWidgets>

// Todo: Make this have an interface like
// getLayerColor(const Material& material) -> QColor
// instead of this mess
QColor getLayerColor(const Layer& layer);
QColor getLayerColor(const LayerProperties& layer);
QColor getLayerColor(double rho, double E);
QPixmap getLayerPixmap(const Layer& layer);
