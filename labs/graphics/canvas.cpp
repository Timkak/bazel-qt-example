#include "canvas.h"

#include "casting/controller.h"

#include <QMouseEvent>
#include <memory>

namespace {
    void DrawPolygons(QPainter& painter, Controller* controller) {
        painter.setPen(QPen(Qt::darkBlue, 2));
        painter.setBrush(QColor(100, 150, 255, 50));
        const auto& polygons = controller->GetPolygons();
        
        for (const auto& polygon : polygons) {
            const auto& vertices = polygon.GetVertices();
            if (vertices.size() < 2) {
                continue;
            }
            for (size_t i = 0; i < vertices.size() - 1; ++i) {
                painter.drawLine(vertices[i], vertices[i + 1]);
            }
            painter.drawLine(vertices.back(), vertices.front());
        }

        if (!polygons.empty()) {
            const auto& last_polygon = polygons.back();
            if (!last_polygon.GetVertices().empty()) {
                const auto& vertices = last_polygon.GetVertices();
                const auto& last_vertex = vertices.back();
                painter.setPen(QPen(Qt::red, 2));
                painter.setBrush(Qt::red);
                painter.drawEllipse(last_vertex, 5, 5);
            }
        }
    }
    void DrawLights(QPainter& painter, Controller* controller) {
        const auto& light_position = controller->GetLightSource();
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::yellow);
        painter.drawEllipse(light_position, 8, 8);
    }
}; // namespace

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    auto new_mode = std::make_unique<LightMode>();
    auto new_controller = std::make_unique<Controller>();
    SetMode(std::move(new_mode));
    SetController(std::move(new_controller));
}

void Canvas::SetMode(std::unique_ptr<CanvasMode> mode) {
    current_mode_ = std::move(mode);
}

void Canvas::SetController(std::unique_ptr<Controller> controller) {
    controller_ = std::move(controller);
}

Controller* Canvas::GetController() const {
    return controller_.get();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (current_mode_) {
        current_mode_->mousePressEvent(event, this);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (current_mode_) {
        current_mode_->mouseMoveEvent(event, this);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (current_mode_) {
        current_mode_->mouseReleaseEvent(event, this);
    }
}

void Canvas::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.fillRect(rect(), Qt::white);
    DrawPolygons(painter, controller_.get());
    DrawLights(painter, controller_.get());
}


void LightMode::mousePressEvent(QMouseEvent* event, Canvas* canvas) {
    if (event->button() == Qt::LeftButton) {
        auto* controller = canvas->GetController();
        controller->SetLightSource(event->pos());
        controller->SetDragging(true);
        canvas->update();
    }
}

void LightMode::mouseMoveEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    if (controller->IsDragging()) {
        controller->SetLightSource(event->pos());
        canvas->update();
    }
}

void LightMode::mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) {
    if (event->button() == Qt::LeftButton) {
        auto* controller = canvas->GetController();
        controller->SetDragging(false);
    }
}

void PolygonMode::mousePressEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    const auto& polygons = controller->GetPolygons();
    switch (event->button()) {
        case Qt::LeftButton:
            if (polygons.empty()) {
                controller->AddPolygon(Polygon());
            }
            controller->AddVertexToLastPolygon(event->pos());
            break;
        case Qt::RightButton:
            if (polygons.empty()) {
                return;
            }
            controller->AddPolygon(Polygon());
            break;
        default:
            return;
    }
    canvas->update();
}

void PolygonMode::mouseMoveEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    if (controller->GetPolygons().empty()) {
        return;
    }
    if (!controller->GetPolygons().back().GetVertices().empty()) {
        controller->UpdateLastPolygon(event->pos());
        canvas->update();
    }
}

void PolygonMode::mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) {
    Q_UNUSED(event);
    Q_UNUSED(canvas);
}