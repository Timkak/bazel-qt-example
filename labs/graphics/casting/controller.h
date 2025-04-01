#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../geom/polygon.h"

#include <QPoint>

#include <vector>

class Controller {
public:
    Controller() = default;
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& new_polygon);
    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);
    [[nodiscard]] QPoint GetLightSource() const;
    void SetLightSource(const QPoint& light_source);
    [[nodiscard]] bool IsDragging() const;
    void SetDragging(bool dragging);
private:
    std::vector<Polygon> polygons_;
    QPoint light_source_;
    bool is_dragging_ = false;
};

#endif