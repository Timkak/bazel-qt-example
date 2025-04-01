#include "canvas.h"

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void Canvas::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
}

void Canvas::SetMode(std::unique_ptr<CanvasMode> mode) {
    current_mode_ = std::move(mode);
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
