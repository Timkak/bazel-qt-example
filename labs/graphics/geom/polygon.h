#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>

#include <optional>
#include <vector>

class Polygon {
public:
    Polygon() = default;
    explicit Polygon(std::vector<QPointF>& vertices);
    [[nodiscard]] std::vector<QPointF> GetVertices() const;
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray& ray) const;
private:
    std::vector<QPointF> vertices_;
};

#endif