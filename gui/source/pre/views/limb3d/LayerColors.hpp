#pragma once
#include <QColor>
#include <QPixmap>
#include <list>

class Layer;
class Material;
class LayerProperties;

QColor getLayerColor(const Layer& layer, const std::list<Material>& materials);
QPixmap getLayerPixmap(const Layer& layer, const std::list<Material>& materials, int size);
QPixmap getColorPixmap(const QColor& color, int size);
