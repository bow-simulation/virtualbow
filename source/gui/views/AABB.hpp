#pragma once
#include <QVector3D>

class AABB
{
public:
    AABB(): AABB(max_inf(), min_inf())
    {

    }

    AABB(const QVector3D& vec_min, const QVector3D& vec_max)
        : vec_min(vec_min), vec_max(vec_max)
    {

    }

    AABB extend(const AABB& other) const
    {
        return {
            min(vec_min, other.vec_min),
            max(vec_max, other.vec_max)
        };
    }

    AABB extend(const QVector3D& point) const
    {
        return {
            min(vec_min, point),
            max(vec_max, point)
        };
    }

    QVector3D center() const
    {
        return (vec_min + vec_max)/2.0f;
    }

    float diagonal() const
    {
        return (vec_max - vec_min).length();
    }

private:
    QVector3D vec_min;
    QVector3D vec_max;

    QVector3D min(const QVector3D& vec1, const QVector3D& vec2) const
    {
        return {
            std::min(vec1.x(), vec2.x()),
            std::min(vec1.y(), vec2.y()),
            std::min(vec1.z(), vec2.z())
        };
    }

    QVector3D max(const QVector3D& vec1, const QVector3D& vec2) const
    {
        return {
            std::max(vec1.x(), vec2.x()),
            std::max(vec1.y(), vec2.y()),
            std::max(vec1.z(), vec2.z())
        };
    }

    QVector3D min_inf() const
    {
        return {
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity()
        };
    }

    QVector3D max_inf() const
    {
        return {
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity()
        };
    }
};
