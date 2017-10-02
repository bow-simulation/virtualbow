#pragma once
#include "fem/elements/ContactElement.hpp"
#include "fem/Element.hpp"
#include "fem/Node.hpp"

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

        bool aabb_intersects(Point p)
        {
            double x_min = std::min(node0[0].u() - h0, node1[0].u() - h1);
            double x_max = std::max(node0[0].u() + h0, node1[0].u() + h1);
            double x = p.node[0].u();

            if(x < x_min || x > x_max)
                return false;

            double y_min = std::min(node0[1].u() - h0, node1[1].u() - h1);
            double y_max = std::max(node0[1].u() + h0, node1[1].u() + h1);
            double y = p.node[1].u();

            if(y < y_min || y > y_max)
                return false;

            return true;
        }
    };

    // Contact side is right hand side of the line of master nodes
    ContactSurface(const std::vector<Node>& master_nodes,
                   const std::vector<Node>& slave_nodes,
                   VectorXd h, double k)
        : k(k)
    {
        assert(master_nodes.size() >= 2);
        assert(master_nodes.size() == h.rows());

        for(size_t i = 1; i < master_nodes.size(); ++i)
            segments.push_back({master_nodes[i-1], master_nodes[i], h[i-1], h[i]});

        for(auto& node: slave_nodes)
            points.push_back({node});
    }

    virtual void update_state() override
    {
        // Todo: Make sure this doesn't change the underlying storage
        contacts.clear();

        // Create a contact element for every combination of segment and point that potentially overlap
        // Todo: Proper broadphase algorthm
        for(auto& s: segments)
        {
            for(auto& p: points)
            {
                if(s.aabb_intersects(p))
                    contacts.push_back({s.node0, s.node1, p.node, s.h0, s.h1, k});
            }
        }

        // Update states
        for(auto& c: contacts)
        {
            c.update_state();
        }
    }

    virtual void get_masses(VectorView M) const override
    {

    }

    virtual void get_internal_forces(VectorView q) const override
    {
        for(auto& c: contacts)
            c.get_internal_forces(q);
    }

    virtual void get_tangent_stiffness(MatrixView K) const override
    {
        for(auto& c: contacts)
            c.get_tangent_stiffness(K);
    }

    virtual double get_potential_energy() const override
    {
        double T = 0.0;
        for(auto& c: contacts)
            T += c.get_potential_energy();
    }

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    std::vector<ContactElement> contacts;
    double k;
};

