#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../geom/polygon.h"

#include <QPointF>
#include <QWidget>

#include <vector>

class Controller {
public:
    Controller() = default; 
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    [[nodiscard]] std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon& new_polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF& light_source);

    [[nodiscard]] bool IsDragging() const;
    void SetDragging(bool dragging);

    [[nodiscard]] bool IsComplete() const;
    void SetComplete(bool complete);

    [[nodiscard]] std::vector<Ray> CastRays(const QPointF& light_source) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    static void RemoveAdjacentRays(std::vector<Ray>* rays);
    [[nodiscard]] std::vector<std::pair<QPointF, Polygon>> CreateLightArea() const;

    [[nodiscard]] const std::vector<QPointF>& GetStaticLights() const;
    [[nodiscard]] std::vector<QPointF>& GetStaticLights();
    void AddStaticLight(const QPointF& new_static_light);
private:
    std::vector<Polygon> polygons_;
    std::vector<QPointF> static_lights_;
    QPointF light_source_;
    bool is_dragging_ = false;
    bool is_complete_ = true;
};

#endif