#ifndef DOCTORAGENT_H
#define DOCTORAGENT_H

#include <QString>

class DoctorAgent {
public:
    static QString getSystemPrompt();
    static QString buildHistoryString(
        const QVector<QPair<QString,QString>>& history);
};

#endif
