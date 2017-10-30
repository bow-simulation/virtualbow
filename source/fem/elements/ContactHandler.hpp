#pragma once
#include "fem/elements/ContactElement.hpp"
#include "fem/Element.hpp"
#include "fem/Node.hpp"
#include "fem/System.hpp"
#include <boost/range/adaptor/map.hpp>

#include <map>

#include <QtCore>

class ContactHandler: public Element
{
public:
    struct Point
    {
        Node node;
    };

    struct Segment
    {
        Node node0;
        Node node1;
        double h0;
        double h1;
    };

    struct Coordinate
    {
        size_t index;
        enum{ SegmentMin, SegmentMax, PointMin, PointMax } type;
        double value;
    };

    bool aabb_intersects(Segment s, Point p) const
    {
        double x_min = std::min(system.get_u(s.node0.x) - s.h0, system.get_u(s.node1.x) - s.h1);
        double x_max = std::max(system.get_u(s.node0.x) + s.h0, system.get_u(s.node1.x) + s.h1);
        double x = system.get_u(p.node.x);

        if(x < x_min || x > x_max)
            return false;

        double y_min = std::min(system.get_u(s.node0.y) - s.h0, system.get_u(s.node1.y) - s.h1);
        double y_max = std::max(system.get_u(s.node0.y) + s.h0, system.get_u(s.node1.y) + s.h1);
        double y = system.get_u(p.node.y);

        if(y < y_min || y > y_max)
            return false;

        return true;
    }

    // Contact side is right hand side of the line of master nodes
    ContactHandler(System& system, ContactForce force)
        : Element(system), force(force)
    {

    }

    void add_segment(const Node& node_a, const Node& node_b, double ha, double hb)
    {
        segments.push_back({node_a, node_b, ha, hb});
        x_list.push_back({segments.size()-1, Coordinate::SegmentMin, 0.0});
        x_list.push_back({segments.size()-1, Coordinate::SegmentMax, 0.0});
        y_list.push_back({segments.size()-1, Coordinate::SegmentMin, 0.0});
        y_list.push_back({segments.size()-1, Coordinate::SegmentMax, 0.0});
    }

    void add_point(const Node& node)
    {
        points.push_back({node});
        x_list.push_back({points.size()-1, Coordinate::PointMin, 0.0});
        y_list.push_back({points.size()-1, Coordinate::PointMax, 0.0});
    }

    void update_coordinates() const
    {
        for(auto& coordinate: x_list)
        {
            switch(coordinate.type)
            {
                case Coordinate::SegmentMin: {
                    const Segment& s = segments[coordinate.index];
                    coordinate.value = std::min(system.get_u(s.node0.x) - s.h0, system.get_u(s.node1.x) - s.h1);
                } break;

                case Coordinate::SegmentMax: {
                    const Segment& s = segments[coordinate.index];
                    coordinate.value = std::max(system.get_u(s.node0.x) + s.h0, system.get_u(s.node1.x) + s.h1);
                } break;

                case Coordinate::PointMin: {
                    const Point& p = points[coordinate.index];
                    coordinate.value = system.get_u(p.node.x);
                } break;

                case Coordinate::PointMax: {
                    const Point& p = points[coordinate.index];
                    coordinate.value = system.get_u(p.node.x);
                } break;
            }
        }

        for(auto& coordinate: y_list)
        {
            switch(coordinate.type)
            {
                case Coordinate::SegmentMin: {
                    const Segment& s = segments[coordinate.index];
                    coordinate.value = std::min(system.get_u(s.node0.y) - s.h0, system.get_u(s.node1.y) - s.h1);
                } break;

                case Coordinate::SegmentMax: {
                    const Segment& s = segments[coordinate.index];
                    coordinate.value = std::max(system.get_u(s.node0.y) + s.h0, system.get_u(s.node1.y) + s.h1);
                } break;

                case Coordinate::PointMin: {
                    const Point& p = points[coordinate.index];
                    coordinate.value = system.get_u(p.node.y);
                } break;

                case Coordinate::PointMax: {
                    const Point& p = points[coordinate.index];
                    coordinate.value = system.get_u(p.node.y);
                } break;
            }
        }
    }

    void sort_axis_list(std::vector<Coordinate>& axis) const
    {
        auto add_contact = [&](size_t i, size_t j) {
            const Segment& s = segments[i];
            const Point& p = points[j];
            contacts.insert({{i, j}, {system, s.node0, s.node1, p.node, s.h0, s.h1, force}});
        };

        auto remove_contact = [&](size_t i, size_t j) {
            contacts.erase({i, j});
        };

        for(int j = 1; j < axis.size(); j++)
        {
            Coordinate keyelement = axis[j];
            double key = keyelement.value;

            int i = j - 1;
            while (i >= 0 && axis[i].value > key)
            {
                Coordinate swapper = axis[i];

                axis[i + 1] = swapper;
                i -= 1;

                if((keyelement.type == Coordinate::SegmentMin || keyelement.type == Coordinate::PointMin)
                  && (swapper.type == Coordinate::SegmentMax || swapper.type == Coordinate::PointMax))
                {
                    if(keyelement.type == Coordinate::SegmentMin && swapper.type == Coordinate::PointMax)
                    {
                        if(aabb_intersects(segments[keyelement.index], points[swapper.index]))
                        {
                            //qInfo() << "Add Pair: Segment " << keyelement.index << ", Point " << swapper.index;
                            add_contact(keyelement.index, swapper.index);
                        }
                    }

                    if(keyelement.type == Coordinate::PointMin && swapper.type == Coordinate::SegmentMax)
                    {
                        if(aabb_intersects(segments[swapper.index], points[keyelement.index]))
                        {
                            //qInfo() << "Add Pair: Segment " << swapper.index << ", Point " << keyelement.index;
                            add_contact(swapper.index, keyelement.index);
                        }
                    }
                }

                if((keyelement.type == Coordinate::SegmentMax || keyelement.type == Coordinate::PointMax)
                        && (swapper.type == Coordinate::SegmentMin || swapper.type == Coordinate::PointMin))
                {
                    if(keyelement.type == Coordinate::SegmentMax && swapper.type == Coordinate::PointMin)
                    {
                        //qInfo() << "Remove Pair: Segment " << keyelement.index << ", Point " << swapper.index;
                        remove_contact(keyelement.index, swapper.index);
                    }


                    if(keyelement.type == Coordinate::PointMin && swapper.type == Coordinate::SegmentMax)
                    {
                        //qInfo() << "Remove Pair: Segment " << swapper.index << ", Point " << keyelement.index;
                        remove_contact(swapper.index, keyelement.index);
                    }
                }
            }

            axis[i + 1] = keyelement;
        }
    }

    void update_contacts() const
    {
        update_coordinates();
        sort_axis_list(x_list);
        sort_axis_list(y_list);

        /*
        for(auto& e: contacts | boost::adaptors::map_keys)
            qInfo() << "Contact: Segment " << e.first << ", Point " << e.second;

        exit(0);
        */
    }

    virtual void add_masses() const override
    {

    }

    virtual void add_internal_forces() const override
    {
        update_contacts();

        for(auto& e: contacts | boost::adaptors::map_values)
            e.add_internal_forces();
    }

    virtual void add_tangent_stiffness() const override
    {
        update_contacts();    // Todo: Avoid Recomputation.

        for(auto& e: contacts | boost::adaptors::map_values)
            e.add_tangent_stiffness();
    }

    virtual double get_potential_energy() const override
    {
        double T = 0.0;
        for(auto& e: contacts | boost::adaptors::map_values)
            T += e.get_potential_energy();
    }

    virtual double get_kinetic_energy() const override
    {
        return 0.0;
    }

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    ContactForce force;

    mutable std::vector<Coordinate> x_list;
    mutable std::vector<Coordinate> y_list;
    mutable std::map<std::pair<size_t, size_t>, ContactElement> contacts;
};

