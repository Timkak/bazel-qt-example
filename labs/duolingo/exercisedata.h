#ifndef EXERCISEDATA_H
#define EXERCISEDATA_H

#include <QString>
#include <QVector>
#include <QMap>

enum class Difficulty {
    Easy,
    Medium,
    Hard
};

struct TranslationTask {
    QString sourceSentence;
    QString correctAnswer;
    QString helpText;
};

struct GrammarTask {
    QString question;
    QStringList options;
    int correctOptionIndex; // 0-based index
    QString helpText;
};

namespace ExerciseData {
    inline QMap<Difficulty, QVector<TranslationTask>> translationTasks;
    inline QMap<Difficulty, QVector<GrammarTask>> grammarTasks;

    inline void initializeData() {
        // --- Easy Translation ---
        translationTasks[Difficulty::Easy] = {
            {"Hello", "Привет", "Common greeting."},
            {"Goodbye", "Пока", "Common farewell."},
            {"Thank you", "Спасибо", "Expressing gratitude."},
            {"Yes", "Да", "Affirmative."},
            {"No", "Нет", "Negative."}
        };

        // --- Medium Translation ---
        translationTasks[Difficulty::Medium] = {
            {"My name is John.", "Меня зовут Джон.", "Introducing yourself."},
            {"How are you?", "Как дела?", "Asking about well-being."},
            {"I am learning Russian.", "Я учу русский язык.", "Describing your activity."},
            {"This is a book.", "Это книга.", "Pointing out an object."},
            {"Where is the library?", "Где библиотека?", "Asking for directions."}
        };
        
        // --- Hard Translation ---
        translationTasks[Difficulty::Hard] = {
            {"The weather is beautiful today.", "Сегодня прекрасная погода.", "Talking about weather."},
            {"Could you please help me?", "Не могли бы вы мне помочь?", "Politely asking for help."},
            {"I would like to order a coffee.", "Я хотел бы заказать кофе.", "Ordering in a cafe."},
            {"This university has a rich history.", "У этого университета богатая история.", "Describing a place."},
            {"Practice makes perfect.", "Повторение - мать учения.", "A common proverb."}
        };


        // --- Easy Grammar ---
        grammarTasks[Difficulty::Easy] = {
            {"I ___ a student. (am/is/are)", {"am", "is", "are"}, 0, "Verb 'to be' for first person singular."},
            {"She ___ reading a book. (am/is/are)", {"am", "is", "are"}, 1, "Verb 'to be' for third person singular."},
            {"They ___ playing. (am/is/are)", {"am", "is", "are"}, 2, "Verb 'to be' for third person plural."},
            {"This ___ an apple. (is/are)", {"is", "are"}, 0, "Singular noun."},
            {"These ___ apples. (is/are)", {"is", "are"}, 1, "Plural noun."}
        };

        // --- Medium Grammar ---
        grammarTasks[Difficulty::Medium] = {
            {"He ___ (go) to school yesterday. (went/goes/gone)", {"went", "goes", "gone"}, 0, "Past Simple tense for regular actions in the past."},
            {"We ___ (study) English now. (study/are studying/studied)", {"study", "are studying", "studied"}, 1, "Present Continuous for actions happening now."},
            {"I have ___ (see) this movie. (see/saw/seen)", {"see", "saw", "seen"}, 2, "Present Perfect for past experiences."},
            {"The cat sleeps ___ the table. (on/in/under)", {"on", "in", "under"}, 0, "Preposition of place."},
            {"She is ___ than her brother. (tall/taller/tallest)", {"tall", "taller", "tallest"}, 1, "Comparative adjective."}
        };
        
        // --- Hard Grammar ---
        grammarTasks[Difficulty::Hard] = {
            {"If I ___ (be) you, I would study more. (am/were/was)", {"am", "were", "was"}, 1, "Subjunctive mood for hypothetical situations (second conditional)."},
            {"By the time he arrived, she ___ (leave). (left/had left/leaves)", {"left", "had left", "leaves"}, 1, "Past Perfect for an action completed before another past action."},
            {"This book, ___ I borrowed from the library, is very interesting. (who/which/whom)", {"who", "which", "whom"}, 1, "Relative pronoun 'which' for objects/things."},
            {"Neither John ___ Mary likes classical music. (or/nor/and)", {"or", "nor", "and"}, 1, "'Neither...nor' correlative conjunction."},
            {"The report must ___ (submit) by tomorrow. (be submitted/submit/submitting)", {"be submitted", "submit", "submitting"}, 0, "Passive voice with modal verb 'must'."}
        };
    }
}
#endif // EXERCISEDATA_H