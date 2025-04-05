// NOLINTBEGIN(cppcoreguidelines-owning-memory)
#include "mainwindow.h"
#include "canvas/canvas.h"

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
    QAction* complete_action = file_menu->addAction("Complete Polygon (ctrl + tab)");
    complete_action->setShortcut(QKeySequence("Ctrl+Tab"));

    // DESIGN
    layout->addWidget(mode_selector_);
    layout->addWidget(canvas_);

    setCentralWidget(central_widget);

    // CONNECTIONS
    connect(mode_selector_, &QComboBox::currentIndexChanged, this, &MainWindow::OnModeChanged);
    connect(mode_selector_, &QComboBox::currentIndexChanged, this, &MainWindow::OnFinishPolygon);
    connect(reset_action, &QAction::triggered, this, &MainWindow::OnCanvasReset);
    connect(complete_action, &QAction::triggered, this, &MainWindow::OnFinishPolygon);
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

void MainWindow::OnFinishPolygon() {
    canvas_->GetController()->FinishPolygon();
    canvas_->update();
}
// NOLINTEND(cppcoreguidelines-owning-memory)
