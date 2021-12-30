#include "LimbMesh.hpp"
#include "LayerColors.hpp"
#include "solver/model/LimbProperties.hpp"
#include "solver/numerics/CubicSpline.hpp"
#include "solver/numerics/Linspace.hpp"


LimbMesh::LimbMesh(const InputData& input)
    : faces_right(GL_QUADS),
      faces_left(GL_QUADS)
{
    try {
        ContinuousLimb limb(input);
        std::vector<double> lengths = getEvalLengths(limb, 1000);    // Magic number

        size_t n_layers = limb.get_layers().size();
        size_t n_segments = lengths.size()-1;

        // Transition points between layers at the left and right of the previous and next cross section
        std::vector<QVector3D> points_l_prev(n_layers+1);
        std::vector<QVector3D> points_l_next(n_layers+1);
        std::vector<QVector3D> points_r_prev(n_layers+1);
        std::vector<QVector3D> points_r_next(n_layers+1);

        std::vector<QColor> colors(n_layers);
        for(size_t i = 0; i < n_layers; ++i) {
            colors[i] = getLayerColor(input.layers[i]);
        }

        // Iterate over segments, i.e. pairs of a previous and a next cross section
        for(size_t i = 0; i < n_segments; ++i) {
            Vector<3> profile_prev = limb.get_r(lengths[i]);
            Vector<3> profile_next = limb.get_r(lengths[i+1]);

            QVector3D center_prev  ( profile_prev[0], profile_prev[1], 0.0 );
            QVector3D normal_w_prev( 0.0, 0.0, 1.0 );
            QVector3D normal_h_prev(-sin(profile_prev[2]), cos(profile_prev[2]), 0.0 );

            QVector3D center_next  ( profile_next[0], profile_next[1], 0.0 );
            QVector3D normal_w_next( 0.0, 0.0, 1.0 );
            QVector3D normal_h_next(-sin(profile_next[2]), cos(profile_next[2]), 0.0 );

            double w_prev = limb.get_w(lengths[i]);
            double w_next = limb.get_w(lengths[i+1]);

            auto y_prev = limb.get_y(lengths[i]);
            auto y_next = limb.get_y(lengths[i+1]);

            points_l_prev.clear();
            points_l_next.clear();
            points_r_prev.clear();
            points_r_next.clear();

            for(size_t j = 0; j < n_layers + 1; ++j) {
                points_r_prev.push_back(center_prev + 0.5*w_prev*normal_w_prev + y_prev[j]*normal_h_prev);
                points_r_next.push_back(center_next + 0.5*w_next*normal_w_next + y_next[j]*normal_h_next);
                points_l_prev.push_back(center_prev - 0.5*w_prev*normal_w_prev + y_prev[j]*normal_h_prev);
                points_l_next.push_back(center_next - 0.5*w_next*normal_w_next + y_next[j]*normal_h_next);
            }

            // Sides
            for(size_t j = 0; j < n_layers; ++j) {
                // Only layers with height != 0
                if(y_prev[j] != y_prev[j+1] || y_next[j] != y_next[j+1]) {
                    // Left
                    addQuad(points_l_prev[j], points_l_next[j], points_l_next[j+1], points_l_prev[j+1], colors[j]);

                    // Right
                    addQuad(points_r_prev[j], points_r_prev[j+1], points_r_next[j+1], points_r_next[j], colors[j]);

                    // Start
                    if(i == 0) {
                        addQuad(points_r_prev[j], points_l_prev[j], points_l_prev[j+1], points_r_prev[j+1], colors[j]);
                    }
                    // End
                    if(i == n_segments - 1) {
                        addQuad(points_r_next[j], points_r_next[j+1], points_l_next[j+1], points_l_next[j], colors[j]);
                    }
                }
            }

            // Back
            for(size_t j = 0; j < n_layers; ++j) {
                // Find first layer with height != 0
                if(y_prev[j] != y_prev[j+1] || y_next[j] != y_next[j+1]) {
                    addQuad(points_l_prev[j], points_r_prev[j], points_r_next[j], points_l_next[j], colors[j]);
                    break;
                }
            }

            // Belly
            for(size_t j = n_layers; j > 0; --j) {
                // Find first layer with height != 0
                if(y_prev[j] != y_prev[j-1] || y_next[j] != y_next[j-1]) {
                    addQuad(points_l_prev[j], points_l_next[j], points_r_next[j], points_r_prev[j], colors[j-1]);
                    break;
                }
            }
        }
    }
    catch(std::invalid_argument&) {

    }
}

std::vector<double> LimbMesh::getEvalLengths(const ContinuousLimb& limb, unsigned n) {
    // Create a sorted vector of all start- and endpoints of the layers.
    // These points must be included in the evaluation lengths.
    std::vector<double> intervals;
    for(auto& layer: limb.get_layers()) {
        intervals.push_back(layer.s_min());
        intervals.push_back(layer.s_max());
    }

    std::sort(intervals.begin(), intervals.end());

    // Calculate evaluation lengths inbetween intervals
    std::vector<double> lengths;
    for(size_t i = 0; i < intervals.size()-1; ++i) {
        unsigned ni = std::ceil(n/limb.length()*(intervals[i+1] - intervals[i]));
        for(unsigned j = 0; j < ni; ++j) {
            double p = double(j)/(ni-1);
            lengths.push_back((1.0 - p)*intervals[i] + p*intervals[i+1]);
        }
    }

    return lengths;
}

void LimbMesh::addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color) {
    QVector3D q0{-p0.x(), p0.y(), p0.z()};
    QVector3D q1{-p3.x(), p3.y(), p3.z()};
    QVector3D q2{-p2.x(), p2.y(), p2.z()};
    QVector3D q3{-p1.x(), p1.y(), p1.z()};

    faces_right.addQuad(p0, p1, p2, p3, color);
    faces_left.addQuad(q0, q1, q2, q3, color);
}
