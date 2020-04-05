#include "LimbMesh.hpp"
#include "LayerColors.hpp"
#include "solver/model//LimbProperties.hpp"
#include "solver/numerics/CubicSpline.hpp"
#include "solver/numerics/Linspace.hpp"

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

        ContinuousLimb limb(data);
        std::vector<double> lengths = getEvalLengths(limb, 150);    // Magic number

        size_t n_layers = limb.get_layers().size();
        size_t n_segments = lengths.size()-1;

        // Transition points between layers at the left and right of the previous and next cross section
        std::vector<QVector3D> points_l_prev(n_layers+1);
        std::vector<QVector3D> points_l_next(n_layers+1);
        std::vector<QVector3D> points_r_prev(n_layers+1);
        std::vector<QVector3D> points_r_next(n_layers+1);

        std::vector<QColor> colors(n_layers);
        for(size_t i = 0; i < n_layers; ++i) {
            colors[i] = getLayerColor(data.layers[i]);
        }

        // Iterate over segments, i.e. pairs of a previous and a next cross section
        for(size_t i = 0; i < n_segments; ++i)
        {
            Vector<3> r_prev = limb.get_r(lengths[i]);
            Vector<3> r_next = limb.get_r(lengths[i+1]);

            QVector3D center_prev  ( r_prev[0], r_prev[1], 0.0 );
            QVector3D normal_w_prev( 0.0, 0.0, 1.0 );
            QVector3D normal_h_prev(-sin(r_prev[2]), cos(r_prev[2]), 0.0 );

            QVector3D center_next  ( r_next[0], r_next[1], 0.0 );
            QVector3D normal_w_next( 0.0, 0.0, 1.0 );
            QVector3D normal_h_next(-sin(r_next[2]), cos(r_next[2]), 0.0 );

            double w_prev = limb.get_w(lengths[i]);
            double w_next = limb.get_w(lengths[i+1]);

            auto y_prev = limb.get_y(lengths[i]);
            auto y_next = limb.get_y(lengths[i+1]);

            points_l_prev.clear();
            points_l_next.clear();
            points_r_prev.clear();
            points_r_next.clear();

            for(size_t j = 0; j < n_layers + 1; ++j)
            {
                points_r_prev.push_back(center_prev + 0.5*w_prev*normal_w_prev + y_prev[j]*normal_h_prev);
                points_r_next.push_back(center_next + 0.5*w_next*normal_w_next + y_next[j]*normal_h_next);
                points_l_prev.push_back(center_prev - 0.5*w_prev*normal_w_prev + y_prev[j]*normal_h_prev);
                points_l_next.push_back(center_next - 0.5*w_next*normal_w_next + y_next[j]*normal_h_next);
            }

            // Sides
            for(size_t j = 0; j < n_layers; ++j)
            {
                // Only layers with height != 0
                if(y_prev[j] != y_prev[j+1] || y_next[j] != y_next[j+1])
                {
                    // Left
                    addQuad(points_l_prev[j], points_l_next[j], points_l_next[j+1], points_l_prev[j+1], colors[j]);

                    // Right
                    addQuad(points_r_prev[j], points_r_prev[j+1], points_r_next[j+1], points_r_next[j], colors[j]);

                    // Start
                    if(i == 0)
                        addQuad(points_r_prev[j], points_l_prev[j], points_l_prev[j+1], points_r_prev[j+1], colors[j]);

                    // End
                    if(i == n_segments - 1)
                        addQuad(points_r_next[j], points_r_next[j+1], points_l_next[j+1], points_l_next[j], colors[j]);
                }
            }

            // Back
            for(size_t j = 0; j < n_layers; ++j)
            {
                // Find first layer with height != 0
                if(y_prev[j] != y_prev[j+1] || y_next[j] != y_next[j+1])
                {
                    addQuad(points_l_prev[j], points_r_prev[j], points_r_next[j], points_l_next[j], colors[j]);
                    break;
                }
            }

            // Belly
            for(size_t j = n_layers; j > 0; --j)
            {
                // Find first layer with height != 0
                if(y_prev[j] != y_prev[j-1] || y_next[j] != y_next[j-1])
                {
                    addQuad(points_l_prev[j], points_l_next[j], points_r_next[j], points_r_prev[j], colors[j-1]);
                    break;
                }
            }
        }
    }
    catch(std::invalid_argument&)
    {
        vertex_data.clear();
        bounding_box = AABB();
    }
}

std::vector<double> LimbMesh::getEvalLengths(const ContinuousLimb& limb, unsigned n)
{
    // Create a sorted vector of all start- and endpoints of the layers.
    // These points must be included in the evaluation lengths.
    std::vector<double> intervals;
    for(auto& layer: limb.get_layers())
    {
        intervals.push_back(layer.s_min());
        intervals.push_back(layer.s_max());
    }

    std::sort(intervals.begin(), intervals.end());

    // Calculate evaluation lengths inbetween intervals
    std::vector<double> lengths;
    for(size_t i = 0; i < intervals.size()-1; ++i)
    {
        unsigned ni = std::ceil(n/(limb.s_max() - limb.s_min())*(intervals[i+1] - intervals[i]));
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

    // Don't use QVector3D::normal() because the implementation arbitrarily decides
    // that a vector with length <= 0.00001 is zero and therefore returns a zero vector.
    // The shader code normalizes the results anyway.
    QVector3D n0 = QVector3D::crossProduct(p1 - p0, p3 - p0);
    QVector3D n1 = QVector3D::crossProduct(p2 - p1, p0 - p1);
    QVector3D n2 = QVector3D::crossProduct(p3 - p2, p1 - p2);
    QVector3D n3 = QVector3D::crossProduct(p0 - p3, p2 - p3);

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
