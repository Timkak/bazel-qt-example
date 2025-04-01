#ifndef RAY_H
#define RAY_H

#include <QPoint>

class Ray {
public:
    Ray(const QPoint& begin, const QPoint& end, double angle);
    [[nodiscard]] QPoint GetBegin() const;
    [[nodiscard]] QPoint GetEnd() const;
    [[nodiscard]] double GetAngle() const;
    void SetBegin(const QPoint& begin);
    void SetEnd(const QPoint& end);
    void SetAngle(double angle);
    [[nodiscard]] Ray Rotate(double angle) const;
private: 
    QPoint begin_;
    QPoint end_;
    double angle_;
};
#endif