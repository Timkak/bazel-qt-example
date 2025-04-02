#include "controller.h"
#include "../geom/misc.h"
#include "../geom/polygon.h"
#include <algorithm>

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

std::vector<Polygon>& Controller::GetPolygons() {
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

const std::vector<QPointF>& Controller::GetStaticLights() const {
    return static_lights_;
}

std::vector<QPointF>& Controller::GetStaticLights() {
    return static_lights_;
}

void Controller::AddStaticLight(const QPointF& new_static_light) {
    static_lights_.push_back(new_static_light);
}

void Controller::UpdateLastStaticLight(const QPoint& new_static_light) {
    static_lights_.back() = new_static_light;
}

std::vector<Ray> Controller::CastRays(const QPointF& light_source) const {
    if (light_source.isNull() || polygons_.empty()) {
        return {};
    }

    std::vector<Ray> casted_rays;
    casted_rays.reserve(polygons_.size() * 3 * 10); // 3 rays per vertex + assume all polygons have at most 10 verteces
    
    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            const auto ray = Ray(light_source, vertex, misc::Angle(vertex - light_source));
            const auto ray2 = ray.Rotate(misc::kEPS2).Scale(misc::kINF);
            const auto ray3 = ray.Rotate(-misc::kEPS2).Scale(misc::kINF);
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
    filtered_rays.push_back(rays->front());

    for (size_t i = 1; i < rays->size(); ++i) {
        const auto& ray1 = rays->at(i);
        const auto& ray = filtered_rays.back();
        double distance = misc::Length(ray1.GetEnd(), ray.GetEnd());
        if (distance > misc::kEPS1) {
            filtered_rays.push_back(ray1);
        }
    }
    rays->swap(filtered_rays);
}


std::vector<std::pair<QPointF, Polygon>> Controller::CreateLightArea() const {
    auto process = [&](const QPointF& light_source) -> Polygon {
        auto rays = CastRays(light_source);
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
    };
    std::vector<std::pair<QPointF, Polygon>> light_area;
    light_area.reserve(static_lights_.size() + 1);
    for (auto light_source : static_lights_) {
        light_area.emplace_back(light_source, process(light_source));
    }
    if (!light_source_.isNull()) {
        light_area.emplace_back(light_source_, process(light_source_));
    }
    return light_area;
}