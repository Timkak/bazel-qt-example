// NOLINTBEGIN(cppcoreguidelines-owning-memory)
#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow() { // NOLINT
    SetupIU();
}

void MainWindow::SetupIU() {
    setWindowTitle("Raycaster");
    auto* central_widget = new QWidget(this);
    auto* layout = new QVBoxLayout(central_widget);
    canvas_ = new Canvas(this);
    layout->addWidget(canvas_);
    setCentralWidget(central_widget);
    resize(600, 500);
}
// NOLINTEND(cppcoreguidelines-owning-memory)