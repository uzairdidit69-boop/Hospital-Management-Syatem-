#ifndef DOCTORCHATSCREEN_H
#define DOCTORCHATSCREEN_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QJsonArray>
#include "../ai/GeminiClient.h"

class DoctorChatScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit DoctorChatScreen(int patientId,   const QString& patientName,
                               int doctorId,   const QString& doctorName,
                               const QString& department,
                               const QString& prescriptionId,
                               int patientAge = 0,
                               const QString& patientGender = "",
                               QWidget* parent = nullptr);
private slots:
    void onSend();
    void onAIResponse(const QString& response);
    void onAIError(const QString& error);

private:
    void buildSystemPrompt();
    void appendChat(const QString& speaker, const QString& msg, const QString& color);
    void applyBackground();
    void resizeEvent(QResizeEvent*) override;
    void processPrescriptionJson(const QString& jsonText);

    int     m_patientId;
    QString m_patientName;
    int     m_doctorId;
    QString m_doctorName;
    QString m_department;
    QString m_prescriptionId;
    int     m_patientAge;
    QString m_patientGender;

    QString    m_systemPrompt;
    QJsonArray m_history;      // [{role, parts}, ...]

    QTextEdit*    chatDisplay;
    QLineEdit*    inputLine;
    QPushButton*  btnSend;
    GeminiClient* gemini;
    QWidget*      centralWidget;
    QWidget*      overlay;
};
#endif
