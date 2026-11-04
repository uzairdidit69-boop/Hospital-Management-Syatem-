#ifndef GEMINICLIENT_H
#define GEMINICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QJsonArray>

class GeminiClient : public QObject {
    Q_OBJECT
public:
    explicit GeminiClient(QObject* parent = nullptr);

    // conversationHistory is a QJsonArray of {role, parts} objects
    void sendMessage(const QString& systemPrompt,
                     const QJsonArray& conversationHistory);

signals:
    void responseReceived(const QString& response);
    void errorOccurred(const QString& error);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* manager;
};

#endif
