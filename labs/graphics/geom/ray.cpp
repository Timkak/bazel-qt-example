#include "ray.h"
#include "misc.h"

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

QPointF Ray::GetBegin() const {
    return begin_;
}

QPointF Ray::GetEnd() const {
    return end_;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetBegin(const QPointF& begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
}

void Ray::SetAngle(double angle) {
    angle_ = angle;
}

Ray Ray::Rotate(double angle) const {
    double new_angle = angle_ + angle; 
    double length = misc::Length(begin_, end_);
    int new_x = static_cast<int>(length * std::cos(new_angle));
    int new_y = static_cast<int>(length * std::sin(new_angle));
    QPointF new_end = begin_ + QPointF(new_x, new_y);
    return {begin_, new_end, new_angle}; 
}