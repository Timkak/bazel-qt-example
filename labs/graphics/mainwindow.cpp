// NOLINTBEGIN(cppcoreguidelines-owning-memory)
#include "mainwindow.h"
#include "canvas.h"

#include <QMenu>
#include <QMenuBar>
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

    QMenuBar* menu_bar = menuBar();
    QMenu* file_menu = menu_bar->addMenu("File");
    QAction* reset_action = file_menu->addAction("Reset Canvas (ctrl + r)");
    reset_action->setShortcut(QKeySequence("Ctrl+R"));
    
    // DESIGN
    layout->addWidget(mode_selector_);
    layout->addWidget(canvas_);

    setCentralWidget(central_widget);

    // CONNECTIONS
    connect(mode_selector_, &QComboBox::currentIndexChanged, this, &MainWindow::OnModeChanged);
    connect(reset_action, &QAction::triggered, this, &MainWindow::OnCanvasReset);
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

void MainWindow::OnCanvasReset() {
    canvas_->ResetCanvas();
    canvas_->SetMode(std::make_unique<LightMode>());
    mode_selector_->setCurrentIndex(0);
}
// NOLINTEND(cppcoreguidelines-owning-memory)