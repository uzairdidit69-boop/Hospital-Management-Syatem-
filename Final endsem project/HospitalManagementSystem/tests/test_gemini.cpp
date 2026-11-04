#include "../ai/GeminiClient.h"
#include "../ai/DoctorAgent.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

static void printResult(const QString& test, bool passed) {
    qDebug().noquote() << QString("[TEST] %-28s : %1").arg(test).arg(passed ? "✅ PASS" : "❌ FAIL");
}

// Helper: synchronously call Gemini and return response (blocks with event loop, 15s timeout)
static QString syncGemini(const QString& system, const QString& history, const QString& msg) {
    GeminiClient client;
    QString result;
    bool done = false;

    QObject::connect(&client, &GeminiClient::responseReceived, [&](const QString& r){
        result = r; done = true;
    });
    QObject::connect(&client, &GeminiClient::errorOccurred, [&](const QString& e){
        result = "__ERROR__: " + e; done = true;
    });

    client.sendMessage(system, history, msg);

    QEventLoop loop;
    QTimer::singleShot(15000, &loop, &QEventLoop::quit);
    while (!done) { loop.processEvents(); }
    return result;
}

void runGeminiTests() {
    qDebug() << "\n========== GEMINI AI TESTS ==========";

    // 1. Connection test
    QString r1 = syncGemini("", "", "Say hello as a doctor in one sentence.");
    bool conn = !r1.isEmpty() && !r1.startsWith("__ERROR__");
    printResult("Gemini Connection", conn);

    // 2. Symptom response
    QString sys = DoctorAgent::getSystemPrompt();
    QString r2  = syncGemini(sys, "", "I have a headache and fever.");
    bool symptom = !r2.isEmpty() && !r2.startsWith("__ERROR__") &&
                   (r2.toLower().contains("fever")     ||
                    r2.toLower().contains("medicine")  ||
                    r2.toLower().contains("rest")       ||
                    r2.toLower().contains("paracetamol")||
                    r2.toLower().contains("symptom")   ||
                    r2.toLower().contains("treatment"));
    printResult("Symptom Response", symptom);

    // 3. Prescription trigger test
    QString history =
        "user: I have had chest pain and shortness of breath for 3 days.\n"
        "model: I understand. Does the pain radiate to your arm or jaw?\n"
        "user: Yes, it goes to my left arm and I feel dizzy.\n"
        "model: Have you had any previous cardiac issues or high blood pressure?\n"
        "user: Yes I have high blood pressure.\n";
    QString r3 = syncGemini(sys, history,
        "Based on all my symptoms, what is your diagnosis and prescription?");
    bool triggered = r3.contains("[PRESCRIPTION_READY]");
    printResult("Prescription Trigger", triggered);

    // 4. JSON prescription
    QString r4 = syncGemini(sys,
        history + "model: [PRESCRIPTION_READY] Based on your symptoms...\n",
        "Please issue the prescription now.");
    // Try to find JSON block
    int s = r4.indexOf('{'), e = r4.lastIndexOf('}');
    bool jsonOk = false;
    if (s >= 0 && e > s) {
        QString json = r4.mid(s, e-s+1);
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        jsonOk = doc.isObject() &&
                 doc.object().contains("medicines") &&
                 doc.object().contains("lab_tests") &&
                 doc.object().contains("operation");
    }
    printResult("JSON Prescription Parse", jsonOk);

    qDebug() << "======================================\n";
}
