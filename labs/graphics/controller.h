#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QPoint>

#include <vector>

class Controller {
public:
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& new_polygon);
    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);
    [[nodiscard]] QPoint GetLightSource() const;
    void SetLightSource(const QPoint& light_source);
private:
    std::vector<Polygon> polygons_;
    QPoint light_source_;
};

#endif