#pragma once
#include <QColor>
#include <QPixmap>

class Layer;
class Material;
class LayerProperties;

QColor getLayerColor(const Layer& layer, const std::vector<Material>& materials);
QPixmap getLayerPixmap(const Layer& layer, const std::vector<Material>& materials, int size);
QPixmap getColorPixmap(const QColor& color, int size);
