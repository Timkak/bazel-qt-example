#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QWidget>

enum class Modes : int8_t {
    light,
    polygons,
};

class Canvas;

class CanvasMode {  // NOLINT

   public:
    virtual ~CanvasMode() = default;
    virtual void mousePressEvent(QMouseEvent* event, Canvas* canvas) = 0;    // NOLINT
    virtual void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) = 0;     // NOLINT
    virtual void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) = 0;  // NOLINT
};

class Canvas : public QWidget {
    Q_OBJECT

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

   public:
    void SetMode(std::unique_ptr<CanvasMode> mode);
    explicit Canvas(QWidget* parent);

   private:
    std::unique_ptr<CanvasMode> current_mode_;
};

class LightMode : public CanvasMode {
private:
    void mousePressEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) override;
};

class PolygonMode : public CanvasMode {
private:
    void mousePressEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) override;
};

#endif