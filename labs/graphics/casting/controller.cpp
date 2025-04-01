#include "controller.h"
#include "../geom/polygon.h"

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& new_polygon) {
    polygons_.emplace_back(new_polygon);
}

void Controller::AddVertexToLastPolygon(const QPoint& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPoint& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

QPoint Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPoint& light_source) {
    light_source_ = light_source;
}