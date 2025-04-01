#ifndef MISC_H
#define MISC_H

#include <QPoint>

namespace misc {
    constexpr double kEPS = 1e-9;
    double DotProduct(const QPoint& v1, const QPoint& v2);
    double Length(const QPoint& p1, const QPoint& p2);
    double Length(const QPoint& v1);
    double CrossProductValue(const QPoint& v1, const QPoint& v2);    
    double Angle(const QPoint& v1);
    bool IsCollinear(const QPoint& v1, const QPoint& v2);
} // namespace misc

#endif