#include "mainwindow.h"

#include "settingsdialog.h"

#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QShortcut>
#include <QShowEvent>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <vector>

int MainWindow::levenshteinDistance(const QString& s1_in, const QString& s2_in) {
    QString s1 = s1_in.trimmed().toLower();
    QString s2 = s2_in.trimmed().toLower();
    const int len1 = s1.length();
    const int len2 = s2.length();
    if (len1 == 0) {
        return len2;
    }
    if (len2 == 0) {
        return len1;
    }
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));
    for (int i = 0; i <= len1; ++i) {
        d[i][0] = i;
    }
    for (int j = 0; j <= len2; ++j) {
        d[0][j] = j;
    }
    for (int j = 1; j <= len2; ++j) {
        for (int i = 1; i <= len1; ++i) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost});
        }
    }
    return d[len1][len2];
}

void MainWindow::setFeedbackStyle(QLabel* label, bool correct) {
    if (!label) {
        return;
    }
    QFont feedbackFont = label->font();
    feedbackFont.setBold(true);
    label->setFont(feedbackFont);
    if (correct) {
        label->setStyleSheet(
            "color: #27ae60; padding: 5px; background-color: #e8f8f5; border-radius: 3px;");
    } else {
        label->setStyleSheet(
            "color: #c0392b; padding: 5px; background-color: #fdedec; border-radius: 3px;");
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , currentDifficulty(Difficulty::Easy)
    , currentExerciseType(ExerciseType::None)
    , tasksCompleted(0)
    , currentMistakes(0)
    , totalScore(0)
    , currentTaskIndex(0)
    , correctSound(new QSoundEffect(this))
    , incorrectSound(new QSoundEffect(this))
    , exerciseStartSound(new QSoundEffect(this))
    , timeUpSound(new QSoundEffect(this))
    , bootUpSound(new QSoundEffect(this))
    , meditateSound(new QSoundEffect(this)) {
    setWindowTitle(tr("LinguaLearn Quest"));
    ExerciseData::initializeData();

    createMenus();
    createMainLayout();
    updateScoreDisplay();
    resetExerciseState();

    exerciseTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    helpShortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
    connect(helpShortcut, &QShortcut::activated, this, &MainWindow::showHelp);

    loadSounds();

    resize(850, 650);
}

MainWindow::~MainWindow() {
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    QMainWindow::keyPressEvent(event);
}

void MainWindow::loadSounds() {
    QString soundPathPrefix = "labs/duolingo/data/";

    bootUpSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "boot_up.wav"));
    correctSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "correct.wav"));
    incorrectSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "incorrect.wav"));
    exerciseStartSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "exercise_start.wav"));
    timeUpSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "time_up.wav"));
    meditateSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "meditation.wav"));

    float volume = 0.6f;
    bootUpSound->setVolume(volume * 0.7f);
    correctSound->setVolume(0.5 * volume);
    incorrectSound->setVolume(volume);
    exerciseStartSound->setVolume(volume * 0.8f);
    timeUpSound->setVolume(volume);
    meditateSound->setVolume(volume * 0.5f);
}

void MainWindow::playSound(QSoundEffect* sound, bool loop) {
    if (sound && sound->isLoaded()) {
        if (sound->isPlaying()) {
            sound->stop();
        }
        sound->setLoopCount(loop ? QSoundEffect::Infinite : 1);
        sound->play();
    } else if (sound) {
        qWarning() << "Sound not loaded or null, cannot play:" << sound->source().toString();
    } else {
        qWarning() << "Attempted to play a null QSoundEffect pointer.";
    }
}

void MainWindow::createMenus() {
    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    changeDifficultyAction = new QAction(tr("&Change Difficulty..."), this);
    connect(changeDifficultyAction, &QAction::triggered, this, &MainWindow::openSettingsDialog);
    settingsMenu->addAction(changeDifficultyAction);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpAction = new QAction(tr("Show &Help (H)"), this);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);
    helpMenu->addAction(helpAction);
}

void MainWindow::createMainLayout() {
    centralWidgetContainer = new QWidget();
    QVBoxLayout* mainVLayout = new QVBoxLayout(centralWidgetContainer);
    mainVLayout->setContentsMargins(15, 15, 15, 15);
    mainVLayout->setSpacing(10);

    QHBoxLayout* topControlsLayout = new QHBoxLayout();
    translationButton = new QPushButton(tr("📚 Translation"));
    grammarSingleButton = new QPushButton(tr("✍️ Grammar (Single)"));
    grammarMultiButton = new QPushButton(tr("✅ Grammar (Multi)"));

    QString exerciseButtonStyle =
        "QPushButton { background-color: #5dade2; color: white; border: none; padding: 10px; "
        "border-radius: 5px; min-width: 160px; font-weight: bold;}"
        "QPushButton:hover { background-color: #3498db; }"
        "QPushButton:disabled { background-color: #aed6f1; }";
    translationButton->setStyleSheet(exerciseButtonStyle);
    grammarSingleButton->setStyleSheet(exerciseButtonStyle);
    grammarMultiButton->setStyleSheet(exerciseButtonStyle);

    topControlsLayout->addWidget(translationButton);
    topControlsLayout->addWidget(grammarSingleButton);
    topControlsLayout->addWidget(grammarMultiButton);
    topControlsLayout->addStretch(1);

    meditateButton = new QPushButton(tr("🧘 Meditate"));
    meditateButton->setStyleSheet(
        "QPushButton { background-color: #1abc9c; color: white; border: none; padding: 10px; "
        "border-radius: 5px; min-width: 120px; font-weight: bold;}"
        "QPushButton:hover { background-color: #16a085; }");
    meditateButton->setCursor(Qt::PointingHandCursor);
    meditateButton->setCheckable(true);
    topControlsLayout->addWidget(meditateButton);
    topControlsLayout->addSpacing(10);

    stopExerciseButton = new QPushButton(tr("Stop Exercise"));
    stopExerciseButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px; "
        "border-radius: 5px; min-width: 120px; font-weight: bold;}"
        "QPushButton:hover { background-color: #c0392b; }"
        "QPushButton:disabled { background-color: #f5b7b1; }");
    stopExerciseButton->setCursor(Qt::PointingHandCursor);
    stopExerciseButton->setVisible(false);
    topControlsLayout->addWidget(stopExerciseButton);

    QHBoxLayout* infoLayout = new QHBoxLayout();
    progressBar = new QProgressBar();
    progressBar->setStyleSheet(
        "QProgressBar { border: 1px solid grey; border-radius: 5px; text-align: center; height: "
        "20px; }"
        "QProgressBar::chunk { background-color: #52be80; width: 10px; margin: 0.5px; "
        "border-radius: 2px; }");
    scoreLabel = new QLabel();
    timerLabel = new QLabel();
    QFont infoFont = scoreLabel->font();
    infoFont.setBold(true);
    infoFont.setPointSize(infoFont.pointSize() + 1);
    scoreLabel->setFont(infoFont);
    timerLabel->setFont(infoFont);
    scoreLabel->setStyleSheet("color: #2c3e50;");
    timerLabel->setStyleSheet("color: #e74c3c;");
    infoLayout->addWidget(new QLabel(tr("🚀 Progress:")));
    infoLayout->addWidget(progressBar, 1);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(new QLabel(tr("⭐ Score:")));
    infoLayout->addWidget(scoreLabel);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(new QLabel(tr("⏳ Time:")));
    infoLayout->addWidget(timerLabel);

    stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet(
        "QWidget { background-color: white; border-radius: 8px; padding: 10px; }");
    welcomeLabel = new QLabel(
        tr("👋 Welcome to LinguaLearn Quest!\n\nChoose an exercise to begin your adventure."));
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setWordWrap(true);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(18);
    welcomeFont.setBold(true);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setStyleSheet("color: #2980b9; background-color: transparent; border: none;");
    translationWidget = new TranslationExerciseWidget();
    grammarSingleWidget = new GrammarExerciseWidget();
    grammarMultiWidget = new MultiCorrectGrammarExerciseWidget();
    stackedWidget->addWidget(welcomeLabel);
    stackedWidget->addWidget(translationWidget);
    stackedWidget->addWidget(grammarSingleWidget);
    stackedWidget->addWidget(grammarMultiWidget);

    statusLabel = new QLabel(tr("Status: Ready to embark on a learning journey!"));
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setMinimumHeight(40);
    statusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusLabel->setStyleSheet(
        "background-color: #eaf2f8; border-radius: 5px; padding: 5px; color: #34495e;");

    mainVLayout->addLayout(topControlsLayout);
    mainVLayout->addLayout(infoLayout);
    mainVLayout->addWidget(stackedWidget, 1);
    mainVLayout->addWidget(statusLabel);

    setCentralWidget(centralWidgetContainer);

    connect(translationButton, &QPushButton::clicked, this, &MainWindow::startTranslationExercise);
    connect(
        grammarSingleButton, &QPushButton::clicked, this, &MainWindow::startGrammarSingleExercise);
    connect(
        grammarMultiButton, &QPushButton::clicked, this, &MainWindow::startGrammarMultiExercise);
    connect(stopExerciseButton, &QPushButton::clicked, this, &MainWindow::stopCurrentExercise);
    connect(meditateButton, &QPushButton::clicked, this, &MainWindow::onMeditateButtonClicked);

    connect(
        translationWidget, &TranslationExerciseWidget::submitClicked, this,
        &MainWindow::handleTranslationSubmit);
    connect(
        grammarSingleWidget, &GrammarExerciseWidget::submitClicked, this,
        &MainWindow::handleGrammarSingleSubmit);
    connect(
        grammarMultiWidget, &MultiCorrectGrammarExerciseWidget::submitClicked, this,
        &MainWindow::handleGrammarMultiSubmit);
}

void MainWindow::openSettingsDialog() {
    SettingsDialog dialog(currentDifficulty, this);
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = dialog.selectedDifficulty();
        statusLabel->setText(tr("Difficulty level updated! Choose your challenge."));
        setFeedbackStyle(statusLabel, true);
        if (currentExerciseType != ExerciseType::None) {
            endExercise(false, tr("Exercise stopped due to difficulty change."));
        }
    }
}

void MainWindow::startTranslationExercise() {
    currentExerciseType = ExerciseType::Translation;
    setupExercise(ExerciseType::Translation);
}

void MainWindow::startGrammarSingleExercise() {
    currentExerciseType = ExerciseType::GrammarSingle;
    setupExercise(ExerciseType::GrammarSingle);
}

void MainWindow::startGrammarMultiExercise() {
    currentExerciseType = ExerciseType::GrammarMulti;
    setupExercise(ExerciseType::GrammarMulti);
}

void MainWindow::setupExercise(ExerciseType type) {
    if (meditateSound && meditateSound->isPlaying()) {
        meditateSound->stop();
        meditateButton->setChecked(false);
        meditateButton->setText(tr("🧘 Meditate"));
        meditateButton->setStyleSheet(
            "QPushButton { background-color: #1abc9c; color: white; border: none; padding: 10px; "
            "border-radius: 5px; min-width: 120px; font-weight: bold;}"
            "QPushButton:hover { background-color: #16a085; }");
    }

    resetExerciseState();
    currentExerciseType = type;
    playSound(exerciseStartSound);

    const auto& tasksForDifficultyTrans = ExerciseData::translationTasks[currentDifficulty];
    const auto& tasksForDifficultyGrammarSingle = ExerciseData::grammarTasks[currentDifficulty];
    const auto& tasksForDifficultyGrammarMulti = ExerciseData::multiGrammarTasks[currentDifficulty];
    int tasksToLoad = 0;

    if (type == ExerciseType::Translation) {
        if (tasksForDifficultyTrans.isEmpty()) {
            QMessageBox::warning(this, tr("No Tasks"), tr("Oops! No translation tasks here..."));
            endExercise(false, "");
            return;
        }
        currentTranslationSet = tasksForDifficultyTrans;
        std::shuffle(
            currentTranslationSet.begin(), currentTranslationSet.end(),
            *QRandomGenerator::global());
        if (currentTranslationSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentTranslationSet.resize(TOTAL_TASKS_PER_EXERCISE);
        }
        tasksToLoad = currentTranslationSet.size();
    } else if (type == ExerciseType::GrammarSingle) {
        if (tasksForDifficultyGrammarSingle.isEmpty()) {
            QMessageBox::warning(
                this, tr("No Tasks"), tr("Hmm, no single-choice grammar tasks..."));
            endExercise(false, "");
            return;
        }
        currentGrammarSingleSet = tasksForDifficultyGrammarSingle;
        std::shuffle(
            currentGrammarSingleSet.begin(), currentGrammarSingleSet.end(),
            *QRandomGenerator::global());
        if (currentGrammarSingleSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentGrammarSingleSet.resize(TOTAL_TASKS_PER_EXERCISE);
        }
        tasksToLoad = currentGrammarSingleSet.size();
    } else if (type == ExerciseType::GrammarMulti) {
        if (tasksForDifficultyGrammarMulti.isEmpty()) {
            QMessageBox::warning(
                this, tr("No Tasks"), tr("Looks like multi-choice grammar tasks..."));
            endExercise(false, "");
            return;
        }
        currentGrammarMultiSet = tasksForDifficultyGrammarMulti;
        std::shuffle(
            currentGrammarMultiSet.begin(), currentGrammarMultiSet.end(),
            *QRandomGenerator::global());
        if (currentGrammarMultiSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentGrammarMultiSet.resize(TOTAL_TASKS_PER_EXERCISE);
        }
        tasksToLoad = currentGrammarMultiSet.size();
    }

    if (tasksToLoad == 0) {
        QString typeStr;
        if (type == ExerciseType::Translation) {
            typeStr = "translation";
        } else if (type == ExerciseType::GrammarSingle) {
            typeStr = "single-choice grammar";
        } else if (type == ExerciseType::GrammarMulti) {
            typeStr = "multi-choice grammar";
        }
        QMessageBox::warning(
            this, tr("No Tasks Loaded"),
            tr("No %1 tasks could be prepared for this session. Please check data or difficulty.")
                .arg(typeStr));
        endExercise(false, "");
        return;
    }
    progressBar->setMaximum(tasksToLoad);
    progressBar->setVisible(true);
    timerLabel->setVisible(true);
    timeRemaining = EXERCISE_TIME_SECONDS;
    timerLabel->setText(QString::number(timeRemaining) + "s");
    timerLabel->setStyleSheet("color: #e74c3c;");
    exerciseTimer->start(1000);

    translationButton->setEnabled(false);
    grammarSingleButton->setEnabled(false);
    grammarMultiButton->setEnabled(false);
    stopExerciseButton->setVisible(true);

    nextTask();
}

void MainWindow::nextTask() {
    QString taskMessage;
    bool taskSet = false;

    if (currentExerciseType == ExerciseType::Translation) {
        if (currentTaskIndex < currentTranslationSet.size()) {
            translationWidget->setTask(currentTranslationSet[currentTaskIndex]);
            stackedWidget->setCurrentWidget(translationWidget);
            taskMessage = tr("Translate this! Task %1 of %2")
                              .arg(currentTaskIndex + 1)
                              .arg(currentTranslationSet.size());
            taskSet = true;
        } else {
            endExercise(true, tr("🎉 Translation set conquered! Well done!"));
            return;
        }
    } else if (currentExerciseType == ExerciseType::GrammarSingle) {
        if (currentTaskIndex < currentGrammarSingleSet.size()) {
            grammarSingleWidget->setTask(currentGrammarSingleSet[currentTaskIndex]);
            stackedWidget->setCurrentWidget(grammarSingleWidget);
            taskMessage = tr("Single Choice Grammar! Task %1 of %2")
                              .arg(currentTaskIndex + 1)
                              .arg(currentGrammarSingleSet.size());
            taskSet = true;
        } else {
            endExercise(true, tr("🎉 Single-choice grammar set mastered! Excellent!"));
            return;
        }
    } else if (currentExerciseType == ExerciseType::GrammarMulti) {
        if (currentTaskIndex < currentGrammarMultiSet.size()) {
            grammarMultiWidget->setTask(currentGrammarMultiSet[currentTaskIndex]);
            stackedWidget->setCurrentWidget(grammarMultiWidget);
            taskMessage = tr("Multi-Choice Challenge! Task %1 of %2")
                              .arg(currentTaskIndex + 1)
                              .arg(currentGrammarMultiSet.size());
            taskSet = true;
        } else {
            endExercise(true, tr("🎉 Multi-choice grammar set aced! Fantastic work!"));
            return;
        }
    }

    if (taskSet) {
        statusLabel->setText(taskMessage);
        statusLabel->setStyleSheet(
            "background-color: #eaf2f8; border-radius: 5px; padding: 5px; color: #34495e;");
    }
}

void MainWindow::handleTranslationSubmit() {
    if (currentExerciseType != ExerciseType::Translation ||
        currentTaskIndex >= currentTranslationSet.size()) {
        return;
    }
    QString userAnswer = translationWidget->getUserAnswer();
    QString correctAnswer = currentTranslationSet[currentTaskIndex].correctAnswer;

    int distance = levenshteinDistance(userAnswer, correctAnswer);
    int shorterLength = qMin(userAnswer.trimmed().length(), correctAnswer.trimmed().length());
    int tolerance =
        (shorterLength > 0) ? static_cast<int>(shorterLength * LEVENSHTEIN_TOLERANCE_PERCENT) : 0;
    if (shorterLength <= 5 && tolerance == 0 && shorterLength > 0) {
        tolerance = 1;
    }
    if (shorterLength == 0 && userAnswer.trimmed().isEmpty() &&
        !correctAnswer.trimmed().isEmpty()) {
        distance = correctAnswer.trimmed().length();
    }

    bool isCorrectByExactMatch =
        (userAnswer.trimmed().toLower() == correctAnswer.trimmed().toLower());
    bool isCorrectByLevenshtein = (!isCorrectByExactMatch && distance <= tolerance);
    bool finalIsCorrect = isCorrectByExactMatch || isCorrectByLevenshtein;

    if (finalIsCorrect) {
        playSound(correctSound);
        tasksCompleted++;
        progressBar->setValue(tasksCompleted);
        currentTaskIndex++;
        if (isCorrectByExactMatch) {
            statusLabel->setText(tr("🌟 Perfect! That's the spot on translation!"));
        } else {
            statusLabel->setText(
                tr("👍 Close enough! The precise answer was: \"%1\"").arg(correctAnswer));
        }
        setFeedbackStyle(statusLabel, true);
        QTimer::singleShot(1800, this, &MainWindow::nextTask);
    } else {
        playSound(incorrectSound);
        currentMistakes++;
        statusLabel->setText(tr("🤔 Not quite. Try %1 of %2. The target was: \"%3\"")
                                 .arg(currentMistakes)
                                 .arg(MAX_MISTAKES_ALLOWED)
                                 .arg(correctAnswer));
        setFeedbackStyle(statusLabel, false);
        translationWidget->clearAnswer();
        if (currentMistakes >= MAX_MISTAKES_ALLOWED) {
            QTimer::singleShot(3000, this, [this]() {
                endExercise(false, tr("Too many attempts. Let's try a new set later!"));
            });
        }
    }
}

void MainWindow::handleGrammarSingleSubmit() {
    if (currentExerciseType != ExerciseType::GrammarSingle ||
        currentTaskIndex >= currentGrammarSingleSet.size()) {
        return;
    }
    int userAnswerIndex = grammarSingleWidget->getSelectedOption();
    int correctAnswerIndex = currentGrammarSingleSet[currentTaskIndex].correctOptionIndex;

    if (userAnswerIndex == -1) {
        statusLabel->setText(tr("⚠️ Please select an option before submitting!"));
        setFeedbackStyle(statusLabel, false);
        return;
    }

    if (userAnswerIndex == correctAnswerIndex) {
        playSound(correctSound);
        tasksCompleted++;
        progressBar->setValue(tasksCompleted);
        currentTaskIndex++;
        statusLabel->setText(tr("🏆 Correct! Excellent choice."));
        setFeedbackStyle(statusLabel, true);
        QTimer::singleShot(1500, this, &MainWindow::nextTask);
    } else {
        playSound(incorrectSound);
        currentMistakes++;
        statusLabel->setText(tr("❌ Oops! That wasn't it. Attempt %1 of %2.")
                                 .arg(currentMistakes)
                                 .arg(MAX_MISTAKES_ALLOWED));
        setFeedbackStyle(statusLabel, false);
        if (currentMistakes >= MAX_MISTAKES_ALLOWED) {
            grammarSingleWidget->showCorrectAnswer(correctAnswerIndex);
            QTimer::singleShot(2500, this, [this]() {
                endExercise(false, tr("Max attempts reached. Time for a new challenge!"));
            });
        }
    }
}

void MainWindow::handleGrammarMultiSubmit() {
    if (currentExerciseType != ExerciseType::GrammarMulti ||
        currentTaskIndex >= currentGrammarMultiSet.size()) {
        return;
    }
    QSet<int> userAnswerIndices = grammarMultiWidget->getSelectedIndices();
    const QSet<int>& correctAnswerIndices =
        currentGrammarMultiSet[currentTaskIndex].correctOptionIndices;

    if (userAnswerIndices.isEmpty()) {
        statusLabel->setText(tr("⚠️ Please select at least one option!"));
        setFeedbackStyle(statusLabel, false);
        return;
    }

    if (userAnswerIndices == correctAnswerIndices) {
        playSound(correctSound);
        tasksCompleted++;
        progressBar->setValue(tasksCompleted);
        currentTaskIndex++;
        statusLabel->setText(tr("🎉 All correct! You nailed it!"));
        setFeedbackStyle(statusLabel, true);
        QTimer::singleShot(1800, this, &MainWindow::nextTask);
    } else {
        playSound(incorrectSound);
        currentMistakes++;
        statusLabel->setText(tr("🧐 Almost! Not all selections were correct. Attempt %1 of %2.")
                                 .arg(currentMistakes)
                                 .arg(MAX_MISTAKES_ALLOWED));
        setFeedbackStyle(statusLabel, false);
        if (currentMistakes >= MAX_MISTAKES_ALLOWED) {
            grammarMultiWidget->showCorrectAnswers(
                correctAnswerIndices, currentGrammarMultiSet[currentTaskIndex].options);
            QTimer::singleShot(3500, this, [this]() {
                endExercise(false, tr("Many tries on this one. Let's move on!"));
            });
        }
    }
}

void MainWindow::onMeditateButtonClicked() {
    if (!meditateSound) {
        return;
    }

    if (meditateButton->isChecked()) {
        playSound(meditateSound, true);
        meditateButton->setText(tr("🧘 Stop Meditating"));
        meditateButton->setStyleSheet(
            "QPushButton { background-color: #16a085; color: white; border: none; padding: 10px; "
            "border-radius: 5px; min-width: 120px; font-weight: bold;}"
            "QPushButton:hover { background-color: #1abc9c; }"

        );
    } else {
        if (meditateSound->isPlaying()) {
            meditateSound->stop();
        }
        meditateButton->setText(tr("🧘 Meditate"));
        meditateButton->setStyleSheet(
            "QPushButton { background-color: #1abc9c; color: white; border: none; padding: 10px; "
            "border-radius: 5px; min-width: 120px; font-weight: bold;}"
            "QPushButton:hover { background-color: #16a085; }");
    }
}

void MainWindow::stopCurrentExercise() {
    if (currentExerciseType != ExerciseType::None) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
            this, tr("Stop Exercise"),
            tr("Are you sure you want to stop the current exercise?\nYour progress in this set "
               "will be lost."),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            endExercise(false, tr("Exercise stopped by user. Back to the main hub!"));
        }
    }
}

void MainWindow::updateTimer() {
    timeRemaining--;
    timerLabel->setText(QString::number(timeRemaining) + "s");
    if (timeRemaining <= 5 && timeRemaining > 0) {
        timerLabel->setStyleSheet("color: #d35400; font-weight: bold;");
    } else if (timeRemaining == 0) {
        timerLabel->setStyleSheet("color: #c0392b; font-weight: bold;");
    }

    if (timeRemaining <= 0) {
        exerciseTimer->stop();
        exerciseTimeUp();
    }
}

void MainWindow::exerciseTimeUp() {
    playSound(timeUpSound);

    endExercise(false, tr("⏰ Time's up! Let's try that again when you're ready."));
}

void MainWindow::endExercise(bool success, const QString& message) {
    exerciseTimer->stop();

    if (meditateSound && meditateSound->isPlaying()) {
        meditateSound->stop();
        meditateButton->setChecked(false);
        meditateButton->setText(tr("🧘 Meditate"));
        meditateButton->setStyleSheet(
            "QPushButton { background-color: #1abc9c; color: white; border: none; padding: 10px; "
            "border-radius: 5px; min-width: 120px; font-weight: bold;}"
            "QPushButton:hover { background-color: #16a085; }");
    }

    ExerciseType endedExerciseType = currentExerciseType;
    currentExerciseType = ExerciseType::None;
    stackedWidget->setCurrentWidget(welcomeLabel);

    bool awardedPoints = false;
    if (success) {
        bool allTasksInSetDone = false;
        if (endedExerciseType == ExerciseType::Translation && !currentTranslationSet.isEmpty()) {
            allTasksInSetDone = (tasksCompleted >= currentTranslationSet.size());
        } else if (
            endedExerciseType == ExerciseType::GrammarSingle &&
            !currentGrammarSingleSet.isEmpty()) {
            allTasksInSetDone = (tasksCompleted >= currentGrammarSingleSet.size());
        } else if (
            endedExerciseType == ExerciseType::GrammarMulti && !currentGrammarMultiSet.isEmpty()) {
            allTasksInSetDone = (tasksCompleted >= currentGrammarMultiSet.size());
        }

        if (allTasksInSetDone && currentMistakes < MAX_MISTAKES_ALLOWED) {
            totalScore += 10;
            awardedPoints = true;
        }
    }

    if (!message.isEmpty()) {
        statusLabel->setText(message);
        setFeedbackStyle(statusLabel, success && awardedPoints);
        QString fullMessage = message;
        if (awardedPoints) {
            fullMessage += tr("\n\n✨ You earned 10 points! Keep it up! ✨");
        }
        QMessageBox::information(
            this, success ? tr("Session Complete!") : tr("Session Ended"), fullMessage);
    }

    updateScoreDisplay();
    resetExerciseState();

    translationButton->setEnabled(true);
    grammarSingleButton->setEnabled(true);
    grammarMultiButton->setEnabled(true);
    stopExerciseButton->setVisible(false);
    meditateButton->setEnabled(true);
}

void MainWindow::updateScoreDisplay() {
    scoreLabel->setText(QString::number(totalScore));
}

void MainWindow::resetExerciseState() {
    tasksCompleted = 0;
    currentMistakes = 0;
    currentTaskIndex = 0;
    progressBar->setValue(0);
    progressBar->setMaximum(TOTAL_TASKS_PER_EXERCISE);
    timerLabel->setText(QString::number(EXERCISE_TIME_SECONDS) + "s");
    timerLabel->setStyleSheet("color: #e74c3c;");

    currentTranslationSet.clear();
    currentGrammarSingleSet.clear();
    currentGrammarMultiSet.clear();

    progressBar->setVisible(false);
    timerLabel->setVisible(false);
    stopExerciseButton->setVisible(false);

    if (translationWidget) {
        translationWidget->clearAnswer();
    }
    if (grammarMultiWidget) {
        grammarMultiWidget->clearSelection();
    }

    if (stackedWidget->currentWidget() != welcomeLabel &&
        currentExerciseType == ExerciseType::None) {
        stackedWidget->setCurrentWidget(welcomeLabel);
    }
    if (currentExerciseType == ExerciseType::None) {
        statusLabel->setText(tr("Status: Ready for your next learning quest!"));
        statusLabel->setStyleSheet(
            "background-color: #eaf2f8; border-radius: 5px; padding: 5px; color: #34495e;");
    }
}

QString MainWindow::getCurrentHelpText() const {
    if (currentExerciseType == ExerciseType::Translation &&
        currentTaskIndex < currentTranslationSet.size()) {
        return currentTranslationSet[currentTaskIndex].helpText;
    } else if (
        currentExerciseType == ExerciseType::GrammarSingle &&
        currentTaskIndex < currentGrammarSingleSet.size()) {
        return currentGrammarSingleSet[currentTaskIndex].helpText;
    } else if (
        currentExerciseType == ExerciseType::GrammarMulti &&
        currentTaskIndex < currentGrammarMultiSet.size()) {
        return currentGrammarMultiSet[currentTaskIndex].helpText;
    }
    return tr(
        "General Help: Select an exercise type to begin. Use 'Settings' to change difficulty. "
        "Press 'H' for specific task help if available during an exercise.");
}

void MainWindow::showHelp() {
    QString helpText = getCurrentHelpText();
    if (helpText.startsWith("General Help:") || (currentExerciseType == ExerciseType::None &&
                                                 stackedWidget->currentWidget() == welcomeLabel)) {
        helpText =
            tr("Welcome to LinguaLearn Quest!\n\n"
               "How to Play:\n"
               "- Pick an exercise: Translation, Single-Choice Grammar, or Multi-Choice Grammar.\n"
               "- Use the 'Settings' menu to adjust the difficulty level.\n"
               "- Your progress through %1 tasks is shown by the progress bar.\n"
               "- You have %2 seconds for each set of tasks.\n"
               "- Be careful! %3 mistakes will end the current exercise set.\n"
               "- You can stop an ongoing exercise using the 'Stop Exercise' button.\n"
               "- Need a break? Click the 'Meditate' button for some calming sounds. You can "
               "meditate even during an exercise!\n"
               "- Press 'H' or use the Help Menu for hints on the current task (if available).")
                .arg(TOTAL_TASKS_PER_EXERCISE)
                .arg(EXERCISE_TIME_SECONDS)
                .arg(MAX_MISTAKES_ALLOWED);
    }
    QMessageBox::information(this, tr("LinguaLearn Quest - Help"), helpText);
}
