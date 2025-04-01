#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPoint>

#include <optional>
#include <vector>

class Polygon {
public:
    explicit Polygon(std::vector<QPoint>& vertices);
    [[nodiscard]] std::vector<QPoint> GetVertices() const;
    void AddVertex(const QPoint& vertex);
    void UpdateLastVertex(const QPoint& new_vertex);
    [[nodiscard]] std::optional<QPoint> IntersectRay(const Ray& ray) const;
private:
    std::vector<QPoint> vertices_;
};

#endif