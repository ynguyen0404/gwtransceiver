#ifndef CJSONPARSER_H
#define CJSONPARSER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariantList>
#include <QVariantMap>

class cJSONParser : public QObject
{
    Q_OBJECT
public:
    explicit cJSONParser(QObject *parent = nullptr);
    ~cJSONParser();
    static QByteArray rawCommandFromServer(QByteArray jsonFromServer);
    static QJsonDocument createJSONToServer(QByteArray data);
    static QJsonDocument createJSONToChirpstackServer(QByteArray data);
};

#endif // CJSONPARSER_H
