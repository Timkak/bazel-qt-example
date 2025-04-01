#include "controller.h"
#include "../geom/misc.h"
#include "../geom/polygon.h"
#include <algorithm>

void Controller::UpdateBorder(const QRect& rect) {
    Polygon border_polygon;
    border_polygon.AddVertex(QPointF(rect.left(), rect.top()));
    border_polygon.AddVertex(QPointF(rect.right(), rect.top()));
    border_polygon.AddVertex(QPointF(rect.right(), rect.bottom()));
    border_polygon.AddVertex(QPointF(rect.left(), rect.bottom()));
    border_polygon.AddVertex(QPointF(rect.left(), rect.top()));
    
    if (polygons_.empty()) {
        polygons_.emplace_back(border_polygon);
    }
    else {
        polygons_.at(0) = border_polygon;
    }

    light_source_ = rect.center();
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& new_polygon) {
    polygons_.emplace_back(new_polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    light_source_ = light_source;
}

bool Controller::IsDragging() const {
    return is_dragging_;
}

void Controller::SetDragging(bool dragging) {
    is_dragging_ = dragging;
}

bool Controller::IsComplete() const {
    return is_complete_;
}

void Controller::SetComplete(bool complete) {
    is_complete_ = complete;
}

std::vector<Ray> Controller::CastRays() const {
    if (light_source_.isNull() || polygons_.empty()) {
        return {};
    }

    std::vector<Ray> casted_rays;
    casted_rays.reserve(polygons_.size() * 3 * 10); // 3 rays per vertex + assume all polygons have at most 10 verteces
    
    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            const auto ray = Ray(light_source_, vertex, misc::Angle(vertex - light_source_));
            const auto ray2 = ray.Rotate(misc::kEPS);
            const auto ray3 = ray.Rotate(-misc::kEPS);
            casted_rays.emplace_back(ray);
            casted_rays.emplace_back(ray2);
            casted_rays.emplace_back(ray3);
        }
    }
    
    return casted_rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    if (rays->empty()) {
        return;
    }
    for (auto& ray : *rays) {
        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (!intersection.has_value()) {
                continue;
            }
            double distance = misc::Length(ray.GetBegin(), intersection.value());
            if (distance < misc::Length(ray.GetBegin(), ray.GetEnd())) {
                ray.SetEnd(intersection.value());
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays){
    if (rays->empty()) {
        return;
    }
    std::sort(rays->begin(), rays->end(), [](const Ray& ray1, const Ray& ray2) { // NOLINT
        return ray1.GetAngle() < ray2.GetAngle();
    });
    std::vector<Ray> filtered_rays;
    filtered_rays.reserve(rays->size());
    filtered_rays.push_back(rays->at(0));
    for (size_t i = 1; i < rays->size(); ++i) {
        const auto& ray1 = rays->at(i);
        const auto& ray = filtered_rays.back();
        const auto& ray0 = filtered_rays.at(0);
        double distance = misc::Length(ray1.GetEnd(), ray.GetEnd());
        double distance0 = misc::Length(ray1.GetEnd(), ray0.GetEnd());
        if (distance > misc::kEPS && distance0 > misc::kEPS) {
            filtered_rays.push_back(ray1);
        }
    }
    rays->swap(filtered_rays);
}

Polygon Controller::CreateLightArea() const {
    auto rays = CastRays();
    if (rays.empty()) {
        return {}; 
    }
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);
    std::vector<QPointF> light_area_points;
    light_area_points.reserve(rays.size());

    std::ranges::transform(rays, std::back_inserter(light_area_points), [](const Ray& ray) { return ray.GetEnd(); });
    
    if (!light_area_points.empty() && light_area_points.front() != light_area_points.back()) {
        light_area_points.push_back(light_area_points.front());
    }
    
    return Polygon(light_area_points);
}