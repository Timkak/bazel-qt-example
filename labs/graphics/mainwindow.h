#ifndef MAINWINDOW
#define MAINWINDOW

#include "canvas.h"

#include <QComboBox>
#include <QMainWindow>
#include <QPushButton>

class MainWindow : public QMainWindow { // NOLINT
    Q_OBJECT
    // todo: try Q_PROPERTY
public:
    MainWindow();
    ~MainWindow() override = default;
private slots:
    void OnModeChanged();
    void OnCanvasReset();

private: // NOLINT 
    void SetupIU();
    
    QComboBox* mode_selector_;
    Canvas* canvas_;
};

#endif