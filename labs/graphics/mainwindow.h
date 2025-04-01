#ifndef MAINWINDOW
#define MAINWINDOW

#include "canvas.h"

#include <QComboBox>
#include <QMainWindow>

class MainWindow : public QMainWindow { // NOLINT
    Q_OBJECT
    // todo: try Q_PROPERTY
public:
    MainWindow();
    ~MainWindow() override = default;
private slots:
    void OnModeChanged();

private: // NOLINT 
    void SetupIU();
    
    QComboBox* mode_selector_;
    Canvas* canvas_;
};

#endif