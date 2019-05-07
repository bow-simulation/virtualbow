#include "LimbMesh.hpp"
#include "LayerColors.hpp"
#include "bow/LimbProperties.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"

LimbMesh::LimbMesh(bool inverted)
    : visible(true),
      inverted(inverted)
{

}

void LimbMesh::setData(const InputData& data)
{
    try
    {
        vertex_data.clear();
        bounding_box = AABB();

        // Todo: Abstract away the conversion data -> profile curve
        std::vector<double> lengths = getEvalLengths(data, 100);
        Curve2D profile = ArcCurve::sample(data.profile,
                                           data.dimensions.handle_length/2.0,
                                           data.dimensions.handle_setback,
                                           data.dimensions.handle_angle,
                                           lengths);

        // Calculate splines for width and heigt distributions
        CubicSpline width(data.width);
        std::vector<CubicSpline> heights;
        for(auto& layer: data.layers)
            heights.push_back(CubicSpline(layer.height));

        size_t n_layers = data.layers.size();
        size_t n_sections = lengths.size()-1;

        std::vector<QVector3D> points_l_prev(n_layers+1);
        std::vector<QVector3D> points_l_next(n_layers+1);
        std::vector<QVector3D> points_r_prev(n_layers+1);
        std::vector<QVector3D> points_r_next(n_layers+1);
        std::vector<size_t> layer_indices(n_layers);

        for(size_t i = 0; i < n_sections; ++i)
        {
            QVector3D center_prev  (profile.x[i], profile.y[i], 0.0 );
            QVector3D normal_w_prev( 0.0, 0.0, 1.0 );
            QVector3D normal_h_prev(-sin(profile.phi[i]), cos(profile.phi[i]), 0.0 );

            QVector3D center_next  ( profile.x[i+1], profile.y[i+1], 0.0 );
            QVector3D normal_w_next( 0.0, 0.0, 1.0 );
            QVector3D normal_h_next(-sin(profile.phi[i+1]), cos(profile.phi[i+1]), 0.0 );

            double p_prev = profile.s[i]/profile.s.maxCoeff();
            double p_next = profile.s[i+1]/profile.s.maxCoeff();

            double w_prev = width(p_prev);
            double w_next = width(p_next);

            points_l_prev.clear();
            points_l_next.clear();
            points_r_prev.clear();
            points_r_next.clear();
            layer_indices.clear();

            auto add_points = [&](double h_prev, double h_next)
            {
                points_r_prev.push_back(center_prev + 0.5*w_prev*normal_w_prev + h_prev*normal_h_prev);
                points_r_next.push_back(center_next + 0.5*w_next*normal_w_next + h_next*normal_h_next);
                points_l_prev.push_back(center_prev - 0.5*w_prev*normal_w_prev + h_prev*normal_h_prev);
                points_l_next.push_back(center_next - 0.5*w_next*normal_w_next + h_next*normal_h_next);
            };

            double h_sum_prev = 0.0;
            double h_sum_next = 0.0;
            for(size_t j = 0; j < n_layers; ++j)
            {
                double h_prev = heights[j](p_prev, 0.0);
                double h_next = heights[j](p_next, 0.0);

                if(h_prev != 0.0 || h_next != 0.0)
                {
                    if(layer_indices.empty())
                        add_points(h_sum_prev, h_sum_next);

                    h_sum_prev -= h_prev;
                    h_sum_next -= h_next;

                    add_points(h_sum_prev, h_sum_next);
                    layer_indices.push_back(j);
                }
            }

            for(size_t j = 0; j < layer_indices.size(); ++j)
            {
                QColor color = getLayerColor(data.layers[layer_indices[j]]);

                // Top side (back)
                if(j == 0)
                    addQuad(points_l_prev[j], points_r_prev[j], points_r_next[j], points_l_next[j], color);

                // Bottom side (belly)
                if(j == layer_indices.size()  - 1)
                    addQuad(points_l_prev[j+1], points_l_next[j+1], points_r_next[j+1], points_r_prev[j+1], color);

                // Limb start
                if(i == 0)
                    addQuad(points_r_prev[j], points_l_prev[j], points_l_prev[j+1], points_r_prev[j+1], color);

                // Limb end
                if(i == n_sections - 1)
                    addQuad(points_r_next[j], points_r_next[j+1], points_l_next[j+1], points_l_next[j], color);

                // Left side
                addQuad(points_l_prev[j], points_l_next[j], points_l_next[j+1], points_l_prev[j+1], color);

                // Right side
                addQuad(points_r_prev[j], points_r_prev[j+1], points_r_next[j+1], points_r_next[j], color);
            }
        }
    }
    catch(std::runtime_error& e)
    {

    }
}

std::vector<double> LimbMesh::getEvalLengths(const InputData& data, unsigned n)
{
    // Todo: Don't use LimbProperties for this?
    LimbProperties limb(data, 150);
    double l = limb.length.maxCoeff();    // Todo: Better way

    // Create a sorted vector of all start- and endpoints of the layers.
    // These points must be included in the evaluation lengths.
    std::vector<double> intervals;
    for(auto& layer: data.layers)
    {
        intervals.push_back(l*layer.height.args().front());
        intervals.push_back(l*layer.height.args().back());
    }
    std::sort(intervals.begin(), intervals.end());

    // Calculate evaluation lengths inbetween intervals
    std::vector<double> lengths;
    for(size_t i = 0; i < intervals.size()-1; ++i)
    {
        unsigned ni = std::ceil(n/l*(intervals[i+1] - intervals[i]));
        for(unsigned j = 0; j < ni; ++j)
        {
            double p = double(j)/(ni-1);
            lengths.push_back((1.0 - p)*intervals[i] + p*intervals[i+1]);
        }
    }

    return lengths;
}

const std::vector<GLfloat>& LimbMesh::vertexData() const
{
    return vertex_data;
}

size_t LimbMesh::vertexCount() const
{
    return vertex_data.size()/9;
}

const AABB LimbMesh::aabb() const
{
    return bounding_box;
}

bool LimbMesh::isVisible() const
{
    return visible;
}

void LimbMesh::setVisible(bool value)
{
    visible = value;
}

void LimbMesh::addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color)
{
    if(inverted)
    {
        p0.setX(-p0.x());
        p1.setX(-p1.x());
        p2.setX(-p2.x());
        p3.setX(-p3.x());
        std::swap(p1, p3);
    }

    QVector3D n0 = QVector3D::normal(p1 - p0, p3 - p0);
    QVector3D n1 = QVector3D::normal(p2 - p1, p0 - p1);
    QVector3D n2 = QVector3D::normal(p3 - p2, p1 - p2);
    QVector3D n3 = QVector3D::normal(p0 - p3, p2 - p3);

    addVertex(p0, n0, color);
    addVertex(p1, n1, color);
    addVertex(p2, n2, color);

    addVertex(p0, n0, color);
    addVertex(p2, n2, color);
    addVertex(p3, n3, color);
}

void LimbMesh::addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color)
{
    vertex_data.push_back(position.x());
    vertex_data.push_back(position.y());
    vertex_data.push_back(position.z());

    vertex_data.push_back(normal.x());
    vertex_data.push_back(normal.y());
    vertex_data.push_back(normal.z());

    vertex_data.push_back(color.redF());
    vertex_data.push_back(color.greenF());
    vertex_data.push_back(color.blueF());

    bounding_box = bounding_box.extend(position);
}
