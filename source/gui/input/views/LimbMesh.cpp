#include "LimbMesh.hpp"
#include "LayerColors.hpp"
#include "bow/LimbProperties.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"
#include <qmath.h>

void LimbMesh::setData(const InputData& data)
{
    m_data.resize(2500*500);
    m_count = 0;

    // Todo: Remove
    /*
    InputData data = data2;
    data.layers = {Layer(), Layer()};
    data.layers[0].height = {{0.0, 0.6}, {0.015, 0.0}};
    data.layers[0].E = 1.0;
    data.layers[1].height = {{0.0, 1.0}, {0.01, 0.01}};
    data.layers[1].E = 2.0;
    */

    // data.layers[2].height = {{0.4, 1.0}, {0.0, 0.01}};
    // data.layers[2].E = 3.0;



    // Todo: Abstract away the conversion data -> profile curve
    std::vector<double> lengths = getEvalLengths(data, 100);
    Curve2D profile = ArcCurve::sample(data.profile,
                                       data.dimensions.handle_length/2.0,
                                       data.dimensions.handle_setback,
                                       data.dimensions.handle_angle,
                                       lengths);

    // Width and heigt distributions
    CubicSpline width(data.width);
    std::vector<CubicSpline> heights;
    for(auto& layer: data.layers)
        heights.push_back(CubicSpline(layer.height));

    unsigned n_layers = data.layers.size();
    unsigned n_sections = lengths.size();

    std::vector<QVector3D> points_l_prev(n_layers + 1);
    std::vector<QVector3D> points_l_next(n_layers + 1);
    std::vector<QVector3D> points_r_prev(n_layers + 1);
    std::vector<QVector3D> points_r_next(n_layers + 1);

    for(unsigned i = 0; i < n_sections; ++i)
    {
        QVector3D center  { profile.x[i], profile.y[i], 0.0 };
        QVector3D normal_w{ 0.0, 0.0, 1.0 };
        QVector3D normal_h{-sin(profile.phi[i]), cos(profile.phi[i]), 0.0 };

        double p = profile.s[i]/profile.s.maxCoeff();
        double w = width(p);
        double h = 0.0;

        for(unsigned j = 0; j < n_layers + 1; ++j)
        {
            points_r_next[j] = center + 0.5*w*normal_w + h*normal_h;
            points_l_next[j] = center - 0.5*w*normal_w + h*normal_h;

            if(j < n_layers)
                h += heights[j](p);
        }

        for(unsigned j = 0; j < n_layers; ++j)
        {
            QColor color = getLayerColor(data.layers[j]);

            if(i == 0)
            {
                addQuad(points_r_next[j], points_l_next[j], points_l_next[j+1], points_r_next[j+1], color);
            }

            if(i == n_sections - 1)
            {
                qInfo() << "Last section";
                addQuad(points_r_next[j], points_r_next[j+1], points_l_next[j+1], points_l_next[j], color);
            }

            if(i > 0)
            {
                // Left
                addQuad(points_l_prev[j], points_l_next[j], points_l_next[j+1], points_l_prev[j+1], color);

                // Right
                addQuad(points_r_prev[j], points_r_prev[j+1], points_r_next[j+1], points_r_next[j], color);

                // Bottom
                if(j == 0)
                {
                    addQuad(points_l_prev[j], points_r_prev[j], points_r_next[j], points_l_next[j], color);
                }

                // Top
                if(j == n_layers  - 1)
                {
                    addQuad(points_l_prev[j+1], points_l_next[j+1], points_r_next[j+1], points_r_prev[j+1], color);
                }
            }
        }

        /*


        if(i > 0)
        {
            // Left
            addQuad(points_l_prev[0], points_l_next[0], points_l_next[1], points_l_prev[1], color);

            // Right
            addQuad(points_r_prev[0], points_r_prev[1], points_r_next[1], points_r_next[0], color);

            // Top
            addQuad(points_l_prev[0], points_r_prev[0], points_r_next[0], points_l_next[0], color);

            // Bottom
            addQuad(points_l_prev[1], points_l_next[1], points_r_next[1], points_r_prev[1], color);
        }
        */

        points_l_prev = points_l_next;
        points_r_prev = points_r_next;
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

const GLfloat* LimbMesh::constData() const
{
    return m_data.constData();
}

int LimbMesh::count() const
{
    return m_count;
}

int LimbMesh::vertexCount() const
{
    return m_count/6;
}

void LimbMesh::addQuad(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QColor& color)
{
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
    GLfloat* p = m_data.data() + m_count;

    *p++ = position.x();
    *p++ = position.y();
    *p++ = position.z();

    *p++ = normal.x();
    *p++ = normal.y();
    *p++ = normal.z();

    *p++ = color.redF();
    *p++ = color.greenF();
    *p++ = color.blueF();

    m_count += 9;
}
