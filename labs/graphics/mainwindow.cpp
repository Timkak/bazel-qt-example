// NOLINTBEGIN(cppcoreguidelines-owning-memory)
#include "mainwindow.h"
#include "canvas.h"

#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow() { // NOLINT
    resize(600, 500);
    SetupIU();
}


void MainWindow::SetupIU() {
    // WIDGETS
    setWindowTitle("Raycaster");
    auto* central_widget = new QWidget(this);
    auto* layout = new QVBoxLayout(central_widget);
    canvas_ = new Canvas(this);

    mode_selector_ = new QComboBox(this);
    mode_selector_->addItems({"Light", "Polygon", "Static Light"});
    mode_selector_->setCurrentIndex(0);

    // DESIGN
    layout->addWidget(mode_selector_);
    layout->addWidget(canvas_);

    setCentralWidget(central_widget);

    // CONNECTIONS
    connect(mode_selector_, &QComboBox::currentIndexChanged, this, &MainWindow::OnModeChanged);
}

void MainWindow::OnModeChanged() {
    std::unique_ptr<CanvasMode> mode;
    
    switch (mode_selector_->currentIndex()) {
        case 0:
            mode = std::make_unique<LightMode>();
            break;
        case 1:
            mode = std::make_unique<PolygonMode>();
            break;
        case 2:
            mode = std::make_unique<StaticLightsMode>();
            break;
        default:
            mode = std::make_unique<LightMode>();
            break;
    }
    
    canvas_->SetMode(std::move(mode));
}
// NOLINTEND(cppcoreguidelines-owning-memory)