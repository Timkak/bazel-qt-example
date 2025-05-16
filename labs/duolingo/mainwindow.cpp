#include "mainwindow.h"
#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QRandomGenerator> // For shuffling tasks
#include <algorithm>     // For std::shuffle
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      currentDifficulty(Difficulty::Easy),
      currentExerciseType(ExerciseType::None),
      tasksCompleted(0),
      currentMistakes(0),
      totalScore(0),
      currentTaskIndex(0)
{
    setWindowTitle(tr("Language Learning App"));
    ExerciseData::initializeData(); // Load exercise data

    createMenus();
    createMainLayout();
    updateScoreDisplay();
    resetExerciseState(); // To set initial state of progress bar etc.

    exerciseTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    helpShortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
    connect(helpShortcut, &QShortcut::activated, this, &MainWindow::showHelp);

    // Set initial size
    resize(800, 600);
}

MainWindow::~MainWindow() {
    // Qt handles child widget deletion
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // This is an alternative to QShortcut if you need more complex key handling
    // if (event->key() == Qt::Key_H) {
    //     showHelp();
    // } else {
    QMainWindow::keyPressEvent(event);
    // }
}


void MainWindow::createMenus() {
    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    changeDifficultyAction = new QAction(tr("&Change Difficulty..."), this);
    connect(changeDifficultyAction, &QAction::triggered, this, &MainWindow::openSettingsDialog);
    settingsMenu->addAction(changeDifficultyAction);

    // You can also add help to a menu if desired
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAction = new QAction(tr("Show &Help"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showHelp);
    helpMenu->addAction(aboutAction);
}

void MainWindow::createMainLayout() {
    centralWidgetContainer = new QWidget();
    QVBoxLayout *mainVLayout = new QVBoxLayout(centralWidgetContainer);

    // Top part: Control Buttons and Info
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    translationButton = new QPushButton(tr("Translation Exercise"));
    grammarButton = new QPushButton(tr("Grammar Exercise"));
    progressBar = new QProgressBar();
    scoreLabel = new QLabel();
    timerLabel = new QLabel();

    controlsLayout->addWidget(translationButton);
    controlsLayout->addWidget(grammarButton);
    controlsLayout->addStretch(1);
    controlsLayout->addWidget(new QLabel(tr("Progress:")));
    controlsLayout->addWidget(progressBar);
    controlsLayout->addWidget(new QLabel(tr("Score:")));
    controlsLayout->addWidget(scoreLabel);
    controlsLayout->addWidget(new QLabel(tr("Time:")));
    controlsLayout->addWidget(timerLabel);

    // Middle part: Stacked Widget for exercises
    stackedWidget = new QStackedWidget();
    welcomeLabel = new QLabel(tr("Welcome! Select an exercise type to begin."));
    welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(16);
    welcomeLabel->setFont(welcomeFont);

    translationWidget = new TranslationExerciseWidget();
    grammarWidget = new GrammarExerciseWidget();

    stackedWidget->addWidget(welcomeLabel); // Index 0
    stackedWidget->addWidget(translationWidget); // Index 1
    stackedWidget->addWidget(grammarWidget);   // Index 2

    // Bottom part: Status messages
    statusLabel = new QLabel(tr("Status: Ready"));
    statusLabel->setAlignment(Qt::AlignCenter);

    mainVLayout->addLayout(controlsLayout);
    mainVLayout->addWidget(stackedWidget, 1); // Give stackedWidget more stretch
    mainVLayout->addWidget(statusLabel);

    setCentralWidget(centralWidgetContainer);

    connect(translationButton, &QPushButton::clicked, this, &MainWindow::startTranslationExercise);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::startGrammarExercise);
    connect(translationWidget, &TranslationExerciseWidget::submitClicked, this, &MainWindow::handleTranslationSubmit);
    connect(grammarWidget, &GrammarExerciseWidget::submitClicked, this, &MainWindow::handleGrammarSubmit);
}

void MainWindow::openSettingsDialog() {
    SettingsDialog dialog(currentDifficulty, this);
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = dialog.selectedDifficulty();
        statusLabel->setText(tr("Difficulty changed. Select an exercise."));
        // Potentially reset ongoing exercise or prevent change during one
        if (currentExerciseType != ExerciseType::None) {
            endExercise(false, tr("Exercise stopped due to difficulty change."));
        }
    }
}

void MainWindow::startTranslationExercise() {
    currentExerciseType = ExerciseType::Translation;
    setupExercise(ExerciseType::Translation);
}

void MainWindow::startGrammarExercise() {
    currentExerciseType = ExerciseType::Grammar;
    setupExercise(ExerciseType::Grammar);
}

void MainWindow::setupExercise(ExerciseType type) {
    resetExerciseState();
    currentExerciseType = type;

    if (type == ExerciseType::Translation) {
        if (ExerciseData::translationTasks[currentDifficulty].isEmpty()) {
            QMessageBox::warning(this, tr("No Tasks"), tr("No translation tasks available for this difficulty."));
            endExercise(false, "");
            return;
        }
        currentTranslationSet = ExerciseData::translationTasks[currentDifficulty];
        std::shuffle(currentTranslationSet.begin(), currentTranslationSet.end(), *QRandomGenerator::global());
        if (currentTranslationSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentTranslationSet.resize(TOTAL_TASKS_PER_EXERCISE);
        } else if (currentTranslationSet.isEmpty()){
             QMessageBox::warning(this, tr("No Tasks"), tr("Not enough translation tasks for this difficulty."));
            endExercise(false, "");
            return;
        }
         progressBar->setMaximum(currentTranslationSet.size());


    } else if (type == ExerciseType::Grammar) {
        if (ExerciseData::grammarTasks[currentDifficulty].isEmpty()) {
            QMessageBox::warning(this, tr("No Tasks"), tr("No grammar tasks available for this difficulty."));
            endExercise(false, "");
            return;
        }
        currentGrammarSet = ExerciseData::grammarTasks[currentDifficulty];
        std::shuffle(currentGrammarSet.begin(), currentGrammarSet.end(), *QRandomGenerator::global());
         if (currentGrammarSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentGrammarSet.resize(TOTAL_TASKS_PER_EXERCISE);
        } else if (currentGrammarSet.isEmpty()){
             QMessageBox::warning(this, tr("No Tasks"), tr("Not enough grammar tasks for this difficulty."));
            endExercise(false, "");
            return;
        }
        progressBar->setMaximum(currentGrammarSet.size());
    }
    
    if ((type == ExerciseType::Translation && currentTranslationSet.isEmpty()) ||
        (type == ExerciseType::Grammar && currentGrammarSet.isEmpty())) {
        QString taskType = (type == ExerciseType::Translation) ? "translation" : "grammar";
        QMessageBox::warning(this, tr("No Tasks"), tr("No %1 tasks available for this difficulty or not enough tasks (need %2).").arg(taskType).arg(TOTAL_TASKS_PER_EXERCISE));
        endExercise(false, "");
        stackedWidget->setCurrentWidget(welcomeLabel);
        return;
    }


    timeRemaining = EXERCISE_TIME_SECONDS;
    timerLabel->setText(QString::number(timeRemaining) + "s");
    exerciseTimer->start(1000); // 1 second interval

    translationButton->setEnabled(false);
    grammarButton->setEnabled(false);

    nextTask();
}

void MainWindow::nextTask() {
    if (currentExerciseType == ExerciseType::Translation) {
        if (currentTaskIndex < currentTranslationSet.size()) {
            translationWidget->setTask(currentTranslationSet[currentTaskIndex]);
            stackedWidget->setCurrentWidget(translationWidget);
            statusLabel->setText(tr("Translation task %1 of %2").arg(currentTaskIndex + 1).arg(currentTranslationSet.size()));
        } else {
            // All tasks completed
            endExercise(true, tr("Translation exercise completed successfully!"));
        }
    } else if (currentExerciseType == ExerciseType::Grammar) {
        if (currentTaskIndex < currentGrammarSet.size()) {
            grammarWidget->setTask(currentGrammarSet[currentTaskIndex]);
            stackedWidget->setCurrentWidget(grammarWidget);
            statusLabel->setText(tr("Grammar task %1 of %2").arg(currentTaskIndex + 1).arg(currentGrammarSet.size()));
        } else {
            // All tasks completed
            endExercise(true, tr("Grammar exercise completed successfully!"));
        }
    }
}

void MainWindow::handleTranslationSubmit() {
    if (currentTaskIndex >= currentTranslationSet.size()) return; // Should not happen

    QString userAnswer = translationWidget->getUserAnswer();
    QString correctAnswer = currentTranslationSet[currentTaskIndex].correctAnswer;

    // Simple case-insensitive comparison for this lab
    if (userAnswer.toLower() == correctAnswer.toLower()) {
        tasksCompleted++;
        progressBar->setValue(tasksCompleted);
        currentTaskIndex++;
        statusLabel->setText(tr("Correct!"));
        QTimer::singleShot(1000, this, &MainWindow::nextTask); // Move to next after a short delay
    } else {
        currentMistakes++;
        statusLabel->setText(tr("Incorrect. Attempt %1 of %2.").arg(currentMistakes).arg(MAX_MISTAKES_ALLOWED));
        translationWidget->clearAnswer(); // Let user try again on the same question.
                                          // Or, you can show the answer and move on.
                                          // For this lab, let's implement the "M mistakes end exercise"
        if (currentMistakes >= MAX_MISTAKES_ALLOWED) {
            translationWidget->showCorrectAnswer(correctAnswer);
             QTimer::singleShot(2000, this, [this](){ // Show answer for a bit
                endExercise(false, tr("Too many mistakes. Exercise failed."));
            });
        }
    }
}

void MainWindow::handleGrammarSubmit() {
    if (currentTaskIndex >= currentGrammarSet.size()) return;

    int userAnswerIndex = grammarWidget->getSelectedOption();
    int correctAnswerIndex = currentGrammarSet[currentTaskIndex].correctOptionIndex;

    if (userAnswerIndex == -1) {
        statusLabel->setText(tr("Please select an option."));
        return;
    }

    if (userAnswerIndex == correctAnswerIndex) {
        tasksCompleted++;
        progressBar->setValue(tasksCompleted);
        currentTaskIndex++;
        statusLabel->setText(tr("Correct!"));
        QTimer::singleShot(1000, this, &MainWindow::nextTask);
    } else {
        currentMistakes++;
        statusLabel->setText(tr("Incorrect. Attempt %1 of %2.").arg(currentMistakes).arg(MAX_MISTAKES_ALLOWED));
        // For grammar, usually we show the answer and move on or fail.
        if (currentMistakes >= MAX_MISTAKES_ALLOWED) {
            grammarWidget->showCorrectAnswer(correctAnswerIndex);
             QTimer::singleShot(2000, this, [this](){
                endExercise(false, tr("Too many mistakes. Exercise failed."));
            });
        }
    }
}

void MainWindow::updateTimer() {
    timeRemaining--;
    timerLabel->setText(QString::number(timeRemaining) + "s");
    if (timeRemaining <= 0) {
        exerciseTimer->stop();
        exerciseTimeUp();
    }
}

void MainWindow::exerciseTimeUp() {
    endExercise(false, tr("Time's up! Exercise failed."));
}

void MainWindow::endExercise(bool success, const QString& message) {
    exerciseTimer->stop();
    currentExerciseType = ExerciseType::None;
    stackedWidget->setCurrentWidget(welcomeLabel);
    statusLabel->setText(message);

    if (success && tasksCompleted > 0 && currentMistakes < MAX_MISTAKES_ALLOWED) {
        // Award points only if all N tasks are done correctly (or within mistake limit if that was the rule)
        // The requirement says "Bally начисляются только в случае корректного выполнения всех N заданий"
        // This implies 0 mistakes for the whole set, or it means successfully completing N tasks.
        // Let's interpret as "successfully completing N tasks with fewer than M total mistakes for the set".
        // If we interpret as "0 mistakes overall", then check `currentMistakes == 0`.
        // For now, `success` flag from `nextTask()` is enough IF it means all tasks were completed.
        // Let's refine: success means all tasks *presented* were answered (correctly).
        // The score is awarded if all tasks in the *set* are completed and total mistakes are low.

        bool allTasksInSetDone = false;
        if(!currentTranslationSet.isEmpty()) allTasksInSetDone = (tasksCompleted == currentTranslationSet.size());
        else if(!currentGrammarSet.isEmpty()) allTasksInSetDone = (tasksCompleted == currentGrammarSet.size());


        if (allTasksInSetDone && currentMistakes < MAX_MISTAKES_ALLOWED) {
             totalScore += 10; // Example score
             QMessageBox::information(this, tr("Exercise Complete"), message + tr("\nYou earned 10 points!"));
        } else if (!message.isEmpty()) {
             QMessageBox::information(this, tr("Exercise Ended"), message);
        }

    } else if (!message.isEmpty()) {
        QMessageBox::information(this, tr("Exercise Ended"), message);
    }


    updateScoreDisplay();
    resetExerciseState(); // Prepare for next possible exercise

    translationButton->setEnabled(true);
    grammarButton->setEnabled(true);
}

void MainWindow::updateScoreDisplay() {
    scoreLabel->setText(QString::number(totalScore));
}

void MainWindow::resetExerciseState() {
    tasksCompleted = 0;
    currentMistakes = 0;
    currentTaskIndex = 0;
    progressBar->setValue(0);
    progressBar->setMaximum(TOTAL_TASKS_PER_EXERCISE); // Default, will be updated
    timerLabel->setText(QString::number(EXERCISE_TIME_SECONDS) + "s");
    currentTranslationSet.clear();
    currentGrammarSet.clear();
    // statusLabel->setText(tr("Status: Ready")); // Done in endExercise or initial setup
    translationWidget->clearAnswer(); // Clear any leftover text
    //grammarWidget->clearSelection(); // Add method to GrammarExerciseWidget if needed to clear radio buttons
}

QString MainWindow::getCurrentHelpText() const {
    if (stackedWidget->currentWidget() == translationWidget && currentTaskIndex < currentTranslationSet.size()) {
        return currentTranslationSet[currentTaskIndex].helpText;
    } else if (stackedWidget->currentWidget() == grammarWidget && currentTaskIndex < currentGrammarSet.size()) {
        return currentGrammarSet[currentTaskIndex].helpText;
    }
    return tr("No specific help available for the current view or task. Try selecting an exercise first.");
}


void MainWindow::showHelp() {
    QString helpText = getCurrentHelpText();
    if (currentExerciseType == ExerciseType::None && stackedWidget->currentWidget() == welcomeLabel) {
        helpText = tr("This is a language learning application.\n"
                      "- Use the 'Settings' menu to change difficulty.\n"
                      "- Click 'Translation Exercise' or 'Grammar Exercise' to start.\n"
                      "- A progress bar shows your progress through a set of %1 tasks.\n"
                      "- You have %2 seconds for each set of tasks.\n"
                      "- Making %3 mistakes will end the exercise.\n"
                      "- Press 'H' for contextual help during an exercise.")
                      .arg(TOTAL_TASKS_PER_EXERCISE)
                      .arg(EXERCISE_TIME_SECONDS)
                      .arg(MAX_MISTAKES_ALLOWED);
    } else if (currentExerciseType == ExerciseType::None) { // Exercise ended but not on welcome screen
         helpText = tr("The exercise has ended. Select a new one or check settings.");
    }


    QMessageBox::information(this, tr("Help"), helpText);
}