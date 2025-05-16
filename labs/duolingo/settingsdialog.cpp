// NOLINTBEGIN(readability-identifier-naming, cppcoreguidelines-owning-memory)

#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

SettingsDialog::SettingsDialog(Difficulty currentDifficulty, QWidget *parent)
    : QDialog(parent), difficultyComboBox(new QComboBox()), m_currentDifficulty(currentDifficulty) {
    setWindowTitle(tr("Settings"));

    auto *mainLayout = new QVBoxLayout(this);

    auto *difficultyLabel = new QLabel(tr("Select Difficulty:"));
    
    difficultyComboBox->addItem(tr("Easy"), QVariant::fromValue(Difficulty::Easy));
    difficultyComboBox->addItem(tr("Medium"), QVariant::fromValue(Difficulty::Medium));
    difficultyComboBox->addItem(tr("Hard"), QVariant::fromValue(Difficulty::Hard));

    // Set current difficulty in combobox
    int currentIndex = 0;
    switch(currentDifficulty) {
        case Difficulty::Easy: currentIndex = 0; break;
        case Difficulty::Medium: currentIndex = 1; break;
        case Difficulty::Hard: currentIndex = 2; break;
    }
    difficultyComboBox->setCurrentIndex(currentIndex);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(difficultyLabel);
    mainLayout->addWidget(difficultyComboBox);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);

    setLayout(mainLayout);
}

Difficulty SettingsDialog::selectedDifficulty() const {
    return difficultyComboBox->currentData().value<Difficulty>();
}
// NOLINTEND(readability-identifier-naming, cppcoreguidelines-owning-memory)