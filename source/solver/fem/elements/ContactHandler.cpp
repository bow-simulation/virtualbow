#include "ContactHandler.hpp"
#include <boost/range/adaptor/map.hpp>

ContactHandler::ContactHandler(System& system, ContactForce force)
    : Element(system), force(force)
{

}

void ContactHandler::add_segment(const Node& node_a, const Node& node_b, double ha, double hb)
{
    x_coordinates.push_back({segments.size(), Coordinate::SegmentMin, 0.0});
    x_coordinates.push_back({segments.size(), Coordinate::SegmentMax, 0.0});

    y_coordinates.push_back({segments.size(), Coordinate::SegmentMin, 0.0});
    y_coordinates.push_back({segments.size(), Coordinate::SegmentMax, 0.0});

    segments.push_back({system, node_a, node_b, ha, hb});
}

void ContactHandler::add_point(const Node& node)
{
    x_coordinates.push_back({points.size(), Coordinate::PointPos, 0.0});
    y_coordinates.push_back({points.size(), Coordinate::PointPos, 0.0});

    points.push_back({system, node});
}

void ContactHandler::update_contacts() const
{
    update_coordinates();
    sort_axis(x_coordinates);
    sort_axis(y_coordinates);
}


// Updates the coordinates in the x and y lists with the current bounding values of the segments and points.
void ContactHandler::update_coordinates() const
{
    for(auto& coordinate: x_coordinates)
    {
        switch(coordinate.type)
        {
        case Coordinate::SegmentMin: coordinate.value = segments[coordinate.index].get_x_min(); break;
        case Coordinate::SegmentMax: coordinate.value = segments[coordinate.index].get_x_max(); break;
        case Coordinate::PointPos  : coordinate.value = points  [coordinate.index].get_x(); break;
        }
    }

    for(auto& coordinate: y_coordinates)
    {
        switch(coordinate.type)
        {
        case Coordinate::SegmentMin: coordinate.value = segments[coordinate.index].get_y_min(); break;
        case Coordinate::SegmentMax: coordinate.value = segments[coordinate.index].get_y_max(); break;
        case Coordinate::PointPos  : coordinate.value = points  [coordinate.index].get_y(); break;
        }
    }
}

// Sorts the coordinates of an axis using insertion sort and creates/removes contact elements
// depending on the occurring swaps. https://en.wikipedia.org/wiki/Insertion_sort
void ContactHandler::sort_axis(std::vector<Coordinate>& coordinates) const
{
    auto add_contact = [&](size_t i, size_t j) {
        contacts.insert({{i, j}, {system, segments[i].node_a, segments[i].node_b,
                                  points[j].node, segments[i].h_a, segments[i].h_b, force}});
    };

    auto remove_contact = [&](size_t i, size_t j) {
        contacts.erase({i, j});
    };

    for(int j = 1; j < coordinates.size(); j++)
    {
        Coordinate key_element = coordinates[j];    // Element that is being placed in it's correct position

        int i = j - 1;
        while(i >= 0 && coordinates[i].value > key_element.value)
        {
            // key_element (right) is being swapped with swap_element (left). There are four cases:
            //
            // key_element is a point position:
            // 1. swap_element is the max of a segment => new overlap on this axis, perform full aabb test and add contact if intersection
            // 2. swap_element is the min of a segment => overlap disappeared, remove contact
            //
            // swap_element is a point position:
            // 3. key_element is the min of a segment => new overlap on this axis, perform full aabb test and add contact if intersection
            // 4. key_element is the max of a segment => overlap disappeared, remove contact

            const Coordinate& swap_element = coordinates[i];

            if(key_element.type == Coordinate::PointPos)
            {
                if(swap_element.type == Coordinate::SegmentMax && points[key_element.index].intersects_aabb(segments[swap_element.index]))
                {
                    add_contact(swap_element.index, key_element.index);
                }
                else if(swap_element.type == Coordinate::SegmentMin)
                {
                    remove_contact(swap_element.index, key_element.index);
                }
            }
            else if(swap_element.type == Coordinate::PointPos)
            {
                if(key_element.type == Coordinate::SegmentMin && points[swap_element.index].intersects_aabb(segments[key_element.index]))
                {
                    add_contact(key_element.index, swap_element.index);
                }
                else if(key_element.type == Coordinate::SegmentMax)
                {
                    remove_contact(key_element.index, swap_element.index);
                }
            }

            coordinates[i + 1] = swap_element;
            i -= 1;
        }

        coordinates[i + 1] = key_element;
    }
}

void ContactHandler::add_masses() const
{

}

void ContactHandler::add_internal_forces() const
{
    update_contacts();

    for(auto& e: contacts | boost::adaptors::map_values)
        e.add_internal_forces();
}

void ContactHandler::add_tangent_stiffness() const
{
    update_contacts();    // Todo: Avoid Recomputation.

    for(auto& e: contacts | boost::adaptors::map_values)
        e.add_tangent_stiffness();
}

void ContactHandler::add_tangent_damping() const
{

}

double ContactHandler::get_potential_energy() const
{
    double T = 0.0;
    for(auto& e: contacts | boost::adaptors::map_values)
        T += e.get_potential_energy();

    return T;
}

double ContactHandler::get_kinetic_energy() const
{
    return 0.0;
}

double ContactHandler::Segment::get_x_min() const
{
    return std::min(system.get_u(node_a.x) - h_a, system.get_u(node_b.x) - h_b);
}

double ContactHandler::Segment::get_x_max() const
{
    return std::max(system.get_u(node_a.x) + h_a, system.get_u(node_b.x) + h_b);
}

double ContactHandler::Segment::get_y_min() const
{
    return std::min(system.get_u(node_a.y) - h_a, system.get_u(node_b.y) - h_b);
}

double ContactHandler::Segment::get_y_max() const
{
    return std::max(system.get_u(node_a.y) + h_a, system.get_u(node_b.y) + h_b);
}

double ContactHandler::Point::get_x() const
{
    return system.get_u(node.x);
}

double ContactHandler::Point::get_y() const
{
    return system.get_u(node.y);
}

bool ContactHandler::Point::intersects_aabb(const Segment& segment) const
{
    return get_x() > segment.get_x_min() && get_x() <= segment.get_x_max() &&
           get_y() > segment.get_y_min() && get_y() <= segment.get_y_max();
}
