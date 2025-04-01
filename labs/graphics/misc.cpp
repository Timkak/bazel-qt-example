#include "misc.h"
#include <cmath>

double misc::Length(const QPoint &p1, const QPoint &p2) { 
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    return std::sqrt((dx * dx) + (dy * dy));
}