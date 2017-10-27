#pragma once
#include "fem/elements/ContactElement.hpp"
#include "fem/Element.hpp"
#include "fem/Node.hpp"
#include "fem/System.hpp"

#include <iostream>

class ContactSurface: public Element
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
    ContactSurface(System& system,
                   const std::vector<Node>& master_nodes,
                   const std::vector<Node>& slave_nodes,
                   const VectorXd& h,
                   ContactForce force)
        : Element(system), force(force)
    {
        assert(master_nodes.size() >= 2);
        assert(master_nodes.size() == h.rows());

        for(size_t i = 1; i < master_nodes.size(); ++i)
            segments.push_back({master_nodes[i-1], master_nodes[i], h[i-1], h[i]});

        for(auto& node: slave_nodes)
            points.push_back({node});
    }

    void update_contacts() const
    {
        // Todo: Make sure this doesn't change the underlying storage
        contacts.clear();

        // Create a contact element for every combination of segment and point that potentially overlap
        // Todo: Proper broadphase algorthm
        for(auto& s: segments)
        {
            for(auto& p: points)
            {
                if(aabb_intersects(s, p))
                    contacts.push_back({system, s.node0, s.node1, p.node, s.h0, s.h1, force});
            }
        }
    }

    virtual void add_masses() const override
    {

    }

    virtual void add_internal_forces() const override
    {
        update_contacts();

        for(auto& c: contacts)
            c.add_internal_forces();
    }

    virtual void add_tangent_stiffness() const override
    {
        update_contacts();    // Todo: Recomputation. Implement better algorithm with temporal coherence.

        for(auto& c: contacts)
            c.add_tangent_stiffness();
    }

    virtual double get_potential_energy() const override
    {
        double T = 0.0;
        for(auto& c: contacts)
            T += c.get_potential_energy();
    }

    virtual double get_kinetic_energy() const override
    {
        return 0.0;
    }

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    ContactForce force;

    mutable std::vector<ContactElement> contacts;
};

