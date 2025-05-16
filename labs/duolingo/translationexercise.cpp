#include "translationexercise.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>

TranslationExerciseWidget::TranslationExerciseWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    instructionLabel = new QLabel(tr("Translate the following sentence:"));
    sentenceLabel = new QLabel();
    sentenceLabel->setWordWrap(true);
    QFont boldFont = sentenceLabel->font();
    boldFont.setBold(true);
    sentenceLabel->setFont(boldFont);

    answerEdit = new QLineEdit();
    submitButton = new QPushButton(tr("Submit"));
    feedbackLabel = new QLabel();
    feedbackLabel->setWordWrap(true);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel(tr("Your translation:")));
    inputLayout->addWidget(answerEdit, 1); // Add stretch factor

    mainLayout->addWidget(instructionLabel);
    mainLayout->addWidget(sentenceLabel);
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch(); // Pushes everything to the top

    connect(submitButton, &QPushButton::clicked, this, &TranslationExerciseWidget::submitClicked);
    connect(answerEdit, &QLineEdit::returnPressed, this, &TranslationExerciseWidget::submitClicked);

    setLayout(mainLayout);
}

void TranslationExerciseWidget::setTask(const TranslationTask& task) {
    sentenceLabel->setText(task.sourceSentence);
    currentCorrectAnswer = task.correctAnswer;
    answerEdit->clear();
    feedbackLabel->clear();
    answerEdit->setFocus();
}

QString TranslationExerciseWidget::getUserAnswer() const {
    return answerEdit->text().trimmed();
}

void TranslationExerciseWidget::clearAnswer() {
    answerEdit->clear();
}

void TranslationExerciseWidget::showCorrectAnswer(const QString& correctAnswer) {
    feedbackLabel->setText(tr("The correct answer was: %1").arg(correctAnswer));
    feedbackLabel->setStyleSheet("color: blue;");
}