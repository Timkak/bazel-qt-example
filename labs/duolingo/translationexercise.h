#ifndef TRANSLATIONEXERCISE_H
#define TRANSLATIONEXERCISE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "exercisedata.h"

class TranslationExerciseWidget : public QWidget {
    Q_OBJECT

public:
    explicit TranslationExerciseWidget(QWidget *parent = nullptr);
    void setTask(const TranslationTask& task);
    QString getUserAnswer() const;
    void clearAnswer();
    void showCorrectAnswer(const QString& correctAnswer); // For showing after attempts exhausted

signals:
    void submitClicked();
    void helpRequested(); // Emitted when user might need help

private:
    QLabel *instructionLabel;
    QLabel *sentenceLabel;
    QLineEdit *answerEdit;
    QPushButton *submitButton;
    QLabel *feedbackLabel; // To show "Correct!" or "Try again" or the correct answer

    QString currentCorrectAnswer;
};

#endif // TRANSLATIONEXERCISE_H