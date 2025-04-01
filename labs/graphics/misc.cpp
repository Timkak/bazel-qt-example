#include "misc.h"

#include <cmath>

double misc::DotProduct(const QPoint& v1, const QPoint& v2) {
    return (v1.x() * v2.x()) + (v1.y() * v2.y());
}

double misc::Length(const QPoint& p1, const QPoint& p2) {
    return std::sqrt(DotProduct(p2 - p1, p2 - p1));
}

double misc::Length(const QPoint& v1) {
    return std::sqrt(DotProduct(v1, v1));
}

double misc::CrossProductValue(const QPoint& v1, const QPoint& v2) {
    return (v1.x() * v2.y()) - (v1.y() * v2.x());
}

bool misc::IsCollinear(const QPoint& v1, const QPoint& v2) {
    return std::abs(CrossProductValue(v1, v2)) < kEPS;
}