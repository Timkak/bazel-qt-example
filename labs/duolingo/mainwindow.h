#ifndef MAINWINDOW_H
#define MAINWINDOW_H
// NOLINTBEGIN(readability-identifier-naming)

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QVector>
#include <QShortcut> // For 'H' key

#include "translationexercise.h"
#include "grammarexercise.h"
#include "exercisedata.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

const int TOTAL_TASKS_PER_EXERCISE = 3; // N
const int MAX_MISTAKES_ALLOWED = 2;   // M
const int EXERCISE_TIME_SECONDS = 120; // 2 minutes per exercise set

enum class ExerciseType {
    None,
    Translation,
    Grammar
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;


private slots:
    void openSettingsDialog();
    void startTranslationExercise();
    void startGrammarExercise();
    void handleTranslationSubmit();
    void handleGrammarSubmit();
    void updateTimer();
    void exerciseTimeUp();
    void showHelp();

private:
    void createMenus();
    void createMainLayout();
    void setupExercise(ExerciseType type);
    void nextTask();
    void endExercise(bool success, const QString& message);
    void updateScoreDisplay();
    void resetExerciseState();
    QString getCurrentHelpText() const;


    // Menu
    QMenu *settingsMenu;
    QAction *changeDifficultyAction;
    QAction *helpAction; // Though 'H' key is primary

    // Main Layout
    QWidget *centralWidgetContainer;
    QStackedWidget *stackedWidget;
    QLabel *welcomeLabel;
    TranslationExerciseWidget *translationWidget;
    GrammarExerciseWidget *grammarWidget;

    // Controls & Info
    QPushButton *translationButton;
    QPushButton *grammarButton;
    QProgressBar *progressBar;
    QLabel *scoreLabel;
    QLabel *timerLabel;
    QLabel *statusLabel; // General feedback

    // Exercise State
    Difficulty currentDifficulty;
    ExerciseType currentExerciseType;
    int tasksCompleted;
    int currentMistakes;
    int totalScore;
    QTimer *exerciseTimer;
    int timeRemaining;

    QVector<TranslationTask> currentTranslationSet;
    QVector<GrammarTask> currentGrammarSet;
    int currentTaskIndex;

    QShortcut *helpShortcut;
};
// NOLINTEND(readability-identifier-naming)

#endif // MAINWINDOW_H