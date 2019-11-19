#pragma once
#include "fem/elements/ContactElement.hpp"
#include "fem/Element.hpp"
#include "fem/Node.hpp"
#include "fem/System.hpp"
#include <map>

// Holds a collection of segments (two nodes, two distances) and points (one node) and creates/removes
// contact elements for them as needed. It uses the Sweep and Prune broadphase algorithm [1],[2] along
// both axes for determining which points and segments can actually come into contact.
//
// [1] https://github.com/mattleibow/jitterphysics/wiki/Sweep-and-Prune
// [2] http://codercorner.com/SAP.pdf
//
// Todo: Dynamic memory allocation when inserting in the map. Maybe try unordered_map?
// It has a reserve method, but iteration should be slower.

class ContactHandler: public Element
{
private:
    struct Segment
    {
        System& system;
        Node node_a;
        Node node_b;
        double h_a;
        double h_b;

        double get_x_min() const;
        double get_x_max() const;
        double get_y_min() const;
        double get_y_max() const;
    };

    struct Point
    {
        System& system;
        Node node;

        double get_x() const;
        double get_y() const;
        bool intersects_aabb(const Segment& segment) const;
    };

    // Represents an interval boundary of a bounding volume along on of the axes, which can
    // be either the minimum/maximum value of a segment or the single value of a point. Holds
    // the index of the associated segment/point.
    struct Coordinate
    {
        size_t index;
        enum{ SegmentMin, SegmentMax, PointPos } type;
        double value;
    };

public:
    ContactHandler(System& system, ContactForce force);
    void add_segment(const Node& node_a, const Node& node_b, double ha, double hb);
    void add_point(const Node& node);

    void update_contacts() const;
    void update_coordinates() const;
    void sort_axis(std::vector<Coordinate>& coordinates) const;

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;
    virtual void add_tangent_damping() const override;

    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    ContactForce force;

    mutable std::vector<Coordinate> x_coordinates;
    mutable std::vector<Coordinate> y_coordinates;
    mutable std::map<std::pair<size_t, size_t>, ContactElement> contacts;
};

