#ifndef CMQTTUTILS_H
#define CMQTTUTILS_H

#include <QObject>
#include <QMqttClient>
#include "cConnectionInfo.h"
#include "cParseConfigureFile.h"
#include <QTimer>
class cMqttUtils : public QObject
{
    Q_OBJECT
public:
    explicit cMqttUtils(QObject *parent = nullptr);
    ~cMqttUtils();
    void connectToServer();
    void disconnectToServer();


private:
    QMqttClient *m_client = nullptr;
    cParseConfigureFile *m_ParseConfigureFile = nullptr;
    cConnectionInfo m_connectionInfo;
signals:
    // Khi nhan command tu server thi parse data va emit signal nay de gui qua uart
    void sendCommandToNode(QByteArray command);
public slots:
    void on_ConnectionStateChange();
    void on_ReceivedMessage(const QByteArray &message, const QMqttTopicName &topic);
    // Khi doc du lieu tu node sex goi slot nay de gui len server
    void on_PublicDataToServer(QByteArray data);
};

#endif // CMQTTUTILS_H
