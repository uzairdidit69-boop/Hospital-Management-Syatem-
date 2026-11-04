#include "GeminiClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// ── Gemini free-tier config ────────────────────────────────────────────────
const QString GEMINI_API_KEY  = "AIzaSyA1r_lfqM4ytrFAEEiNEbMcxPWwz-D0V9Q";
const QString GEMINI_MODEL    = "gemini-2.5-flash";   // free tier — 500 req/day
const QString GEMINI_BASE_URL = "https://generativelanguage.googleapis.com/v1beta/models/";
// ──────────────────────────────────────────────────────────────────────────

GeminiClient::GeminiClient(QObject* parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            this, &GeminiClient::onReplyFinished);
}

void GeminiClient::sendMessage(const QString& systemPrompt,
                               const QJsonArray& conversationHistory)
{
    // ── systemInstruction (top-level, separate from contents) ──
    QJsonObject sysPart;
    sysPart["text"] = systemPrompt;
    QJsonObject sysInstr;
    sysInstr["parts"] = QJsonArray{ sysPart };

    QJsonObject body;
    body["systemInstruction"] = sysInstr;
    body["contents"]          = conversationHistory;

    QUrl url(GEMINI_BASE_URL + GEMINI_MODEL + ":generateContent?key=" + GEMINI_API_KEY);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    manager->post(req, QJsonDocument(body).toJson());
}

void GeminiClient::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        QByteArray err = reply->readAll();
        emit errorOccurred(reply->errorString() + "\n" + QString::fromUtf8(err));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray candidates = doc.object()["candidates"].toArray();
    if (candidates.isEmpty()) {
        emit errorOccurred("Empty response from Gemini.");
        return;
    }

    QString text = candidates[0].toObject()["content"]
                   .toObject()["parts"]
                   .toArray()[0]
                   .toObject()["text"].toString();

    emit responseReceived(text);
}
