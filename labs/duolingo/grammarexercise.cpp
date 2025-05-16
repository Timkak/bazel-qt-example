#include "grammarexercise.h"
#include <QVBoxLayout>
#include <QButtonGroup> // To manage radio buttons

GrammarExerciseWidget::GrammarExerciseWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    instructionLabel = new QLabel(tr("Choose the correct option to complete the sentence:"));
    questionLabel = new QLabel();
    questionLabel->setWordWrap(true);
    QFont boldFont = questionLabel->font();
    boldFont.setBold(true);
    questionLabel->setFont(boldFont);

    optionsGroup = new QGroupBox(tr("Options"));
    optionsLayout = new QVBoxLayout(); // Will hold radio buttons
    optionsGroup->setLayout(optionsLayout);

    submitButton = new QPushButton(tr("Submit"));
    feedbackLabel = new QLabel();
    feedbackLabel->setWordWrap(true);

    mainLayout->addWidget(instructionLabel);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch();

    connect(submitButton, &QPushButton::clicked, this, &GrammarExerciseWidget::submitClicked);

    setLayout(mainLayout);
}

void GrammarExerciseWidget::setTask(const GrammarTask& task) {
    questionLabel->setText(task.question);
    currentCorrectOptionIndex = task.correctOptionIndex;
    feedbackLabel->clear();

    // Clear previous radio buttons
    qDeleteAll(radioButtons);
    radioButtons.clear();
    QLayoutItem *child;
    while ((child = optionsLayout->takeAt(0)) != nullptr) {
        delete child->widget(); // delete the widget
        delete child;           // delete the layout item
    }


    for (const QString& optionText : task.options) {
        QRadioButton *rb = new QRadioButton(optionText);
        radioButtons.append(rb);
        optionsLayout->addWidget(rb);
    }

    if (!radioButtons.isEmpty()) {
        radioButtons.first()->setChecked(true); // Select first by default
        radioButtons.first()->setFocus();
    }
}

int GrammarExerciseWidget::getSelectedOption() const {
    for (int i = 0; i < radioButtons.size(); ++i) {
        if (radioButtons[i]->isChecked()) {
            return i;
        }
    }
    return -1; // No option selected
}

void GrammarExerciseWidget::showCorrectAnswer(int correctIndex) {
    if (correctIndex >= 0 && correctIndex < radioButtons.size()) {
        feedbackLabel->setText(tr("The correct option was: %1").arg(radioButtons[correctIndex]->text()));
        feedbackLabel->setStyleSheet("color: blue;");
    }
}