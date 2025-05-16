#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "exercisedata.h"
#include "grammarexercise.h"
#include "multicorrectgrammarexercise.h"
#include "translationexercise.h"

#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QShortcut>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QTimer>
#include <QVector>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPushButton;
QT_END_NAMESPACE

const int TOTAL_TASKS_PER_EXERCISE = 3;
const int MAX_MISTAKES_ALLOWED = 2;
const int EXERCISE_TIME_SECONDS = 120;

enum class ExerciseType { None, Translation, GrammarSingle, GrammarMulti };

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:
    void keyPressEvent(QKeyEvent* event) override;

   private slots:
    void openSettingsDialog();
    void startTranslationExercise();
    void startGrammarSingleExercise();
    void startGrammarMultiExercise();
    void handleTranslationSubmit();
    void handleGrammarSingleSubmit();
    void handleGrammarMultiSubmit();
    void updateTimer();
    void exerciseTimeUp();
    void showHelp();
    void stopCurrentExercise();
    void onMeditateButtonClicked();

   private:
    void createMenus();
    void createMainLayout();
    void setupExercise(ExerciseType type);
    void nextTask();
    void endExercise(bool success, const QString& message);
    void updateScoreDisplay();
    void resetExerciseState();
    QString getCurrentHelpText() const;

    static int levenshteinDistance(const QString& s1, const QString& s2);
    const double LEVENSHTEIN_TOLERANCE_PERCENT = 0.20;

    void setFeedbackStyle(QLabel* label, bool correct);

    void loadSounds();
    void playSound(QSoundEffect* sound, bool loop = false);
    QSoundEffect* correctSound;
    QSoundEffect* incorrectSound;
    QSoundEffect* exerciseStartSound;
    QSoundEffect* timeUpSound;
    QSoundEffect* bootUpSound;
    QSoundEffect* meditateSound;

    QMenu* settingsMenu;
    QAction* changeDifficultyAction;
    QAction* helpAction;

    QWidget* centralWidgetContainer;
    QStackedWidget* stackedWidget;
    QLabel* welcomeLabel;
    TranslationExerciseWidget* translationWidget;
    GrammarExerciseWidget* grammarSingleWidget;
    MultiCorrectGrammarExerciseWidget* grammarMultiWidget;

    QPushButton* translationButton;
    QPushButton* grammarSingleButton;
    QPushButton* grammarMultiButton;
    QPushButton* stopExerciseButton;
    QPushButton* meditateButton;
    QProgressBar* progressBar;
    QLabel* scoreLabel;
    QLabel* timerLabel;
    QLabel* statusLabel;

    Difficulty currentDifficulty;
    ExerciseType currentExerciseType;
    int tasksCompleted;
    int currentMistakes;
    int totalScore;
    QTimer* exerciseTimer;
    int timeRemaining;

    QVector<TranslationTask> currentTranslationSet;
    QVector<GrammarTask> currentGrammarSingleSet;
    QVector<MultiCorrectGrammarTask> currentGrammarMultiSet;
    int currentTaskIndex;

    QShortcut* helpShortcut;
};

#endif