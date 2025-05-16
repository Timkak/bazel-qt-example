#ifndef EXERCISEDATA_H
#define EXERCISEDATA_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>

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
    int correctOptionIndex;
    QString helpText;
};

struct MultiCorrectGrammarTask {
    QString question;
    QStringList options;
    QSet<int> correctOptionIndices;
    QString helpText;
};


namespace ExerciseData {
    inline QMap<Difficulty, QVector<TranslationTask>> translationTasks;
    inline QMap<Difficulty, QVector<GrammarTask>> grammarTasks;
    inline QMap<Difficulty, QVector<MultiCorrectGrammarTask>> multiGrammarTasks;

    inline void initializeData() {
        translationTasks[Difficulty::Easy] = {
            {"Hello", "Привет", "Common greeting."},
            {"Goodbye", "Пока", "Common farewell."},
            {"Thank you", "Спасибо", "Expressing gratitude."},
            {"Yes", "Да", "Affirmative."},
            {"No", "Нет", "Negative."},
            {"Please", "Пожалуйста", "Used for requests or offering something."},
            {"Good morning", "Доброе утро", "Morning greeting."},
            {"Water", "Вода", "A common noun."}
        };

        translationTasks[Difficulty::Medium] = {
            {"My name is John.", "Меня зовут Джон.", "Introducing yourself."},
            {"How are you?", "Как дела?", "Asking about well-being."},
            {"I am learning Russian.", "Я учу русский язык.", "Describing your activity."},
            {"This is a book.", "Это книга.", "Pointing out an object."},
            {"Where is the library?", "Где библиотека?", "Asking for directions."},
            {"What time is it?", "Который час?", "Asking for the time."},
            {"I don't understand.", "Я не понимаю.", "Expressing lack of understanding."}
        };

        translationTasks[Difficulty::Hard] = {
            {"The weather is beautiful today.", "Сегодня прекрасная погода.", "Talking about weather."},
            {"Could you please help me?", "Не могли бы вы мне помочь?", "Politely asking for help."},
            {"I would like to order a coffee.", "Я хотел бы заказать кофе.", "Ordering in a cafe."},
            {"This university has a rich history.", "У этого университета богатая история.", "Describing a place."},
            {"Practice makes perfect.", "Повторение - мать учения.", "A common proverb."},
            {"To kill two birds with one stone.", "Убить двух зайцев одним выстрелом.", "Idiomatic expression."},
            {"Never judge a book by its cover.", "Не суди книгу по обложке.", "A piece of advice."}
        };


        grammarTasks[Difficulty::Easy] = {
            {"I ___ a student. (am/is/are)", {"am", "is", "are"}, 0, "Verb 'to be' for first person singular."},
            {"She ___ reading a book. (am/is/are)", {"am", "is", "are"}, 1, "Verb 'to be' for third person singular."},
            {"They ___ playing. (am/is/are)", {"am", "is", "are"}, 2, "Verb 'to be' for third person plural."},
            {"This ___ an apple. (is/are)", {"is", "are"}, 0, "Singular noun."},
            {"These ___ apples. (is/are)", {"is", "are"}, 1, "Plural noun."},
            {"He ___ a car. (has/have)", {"has", "have"}, 0, "Possession for third person singular."}
        };

        grammarTasks[Difficulty::Medium] = {
            {"He ___ (go) to school yesterday. (went/goes/gone)", {"went", "goes", "gone"}, 0, "Past Simple tense."},
            {"We ___ (study) English now. (study/are studying/studied)", {"study", "are studying", "studied"}, 1, "Present Continuous tense."},
            {"I have ___ (see) this movie. (see/saw/seen)", {"see", "saw", "seen"}, 2, "Present Perfect tense."},
            {"The cat sleeps ___ the table. (on/in/under)", {"on", "in", "under"}, 0, "Preposition of place."},
            {"She is ___ than her brother. (tall/taller/tallest)", {"tall", "taller", "tallest"}, 1, "Comparative adjective."}
        };

        grammarTasks[Difficulty::Hard] = {
            {"If I ___ (be) you, I would study more. (am/were/was)", {"am", "were", "was"}, 1, "Subjunctive mood (second conditional)."},
            {"By the time he arrived, she ___ (leave). (left/had left/leaves)", {"left", "had left", "leaves"}, 1, "Past Perfect tense."},
            {"This book, ___ I borrowed from the library, is very interesting. (who/which/whom)", {"who", "which", "whom"}, 1, "Relative pronoun for objects."},
            {"Neither John ___ Mary likes classical music. (or/nor/and)", {"or", "nor", "and"}, 1, "'Neither...nor' conjunction."},
            {"The report must ___ (submit) by tomorrow. (be submitted/submit/submitting)", {"be submitted", "submit", "submitting"}, 0, "Passive voice with modal."}
        };

        multiGrammarTasks[Difficulty::Easy] = {
            {"Which of these are fruits? (Select all that apply)", {"Apple", "Carrot", "Banana", "Broccoli"}, {0, 2}, "Identify common fruits."},
            {"Select the primary colors:", {"Red", "Green", "Blue", "Yellow (mixed)"}, {0, 2}, "Primary colors in RGB model (often red, yellow, blue in subtractive pigment model, but for simplicity let's use common understanding)."},
            {"Which words are nouns?", {"Quickly", "Table", "Jump", "Happiness"}, {1, 3}, "Nouns are names of things, places, people, or ideas."}
        };

        multiGrammarTasks[Difficulty::Medium] = {
            {"Select the words that can be adjectives:", {"Beauty", "Beautiful", "Beautifully", "Beautify"}, {1}, "Adjectives describe nouns. Note: 'Beautiful' is the only direct adjective here."},
            {"Which sentences are in the Past Simple tense?", {"She eats an apple.", "He walked home.", "They were playing.", "I saw a bird."}, {1, 3}, "Past Simple describes completed actions in the past."},
            {"Select all synonyms for 'happy':", {"Joyful", "Sad", "Content", "Angry", "Pleased"}, {0, 2, 4}, "Synonyms are words with similar meanings."}
        };
        multiGrammarTasks[Difficulty::Hard] = {
            {"Which of these are renewable energy sources? (Select all that apply)", {"Solar", "Coal", "Wind", "Natural Gas", "Geothermal"}, {0, 2, 4}, "Renewable energy sources replenish naturally."},
            {"Select all modal verbs from the list:", {"Can", "Must", "Eat", "Should", "Sleep", "Might"}, {0, 1, 3, 5}, "Modal verbs express modality like possibility, obligation, etc."},
            {"Identify all sentences written in the passive voice:",
                {"The cake was eaten by the dog.", "The dog ate the cake.", "A decision will be made soon.", "She is writing a letter."},
                {0, 2}, "In passive voice, the subject receives the action."
            }
        };
    }
}
#endif