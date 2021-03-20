#ifndef CMQTTUTILS_H
#define CMQTTUTILS_H

#include <QObject>
#include <QThread>
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

    cMqttUtils (const cMqttUtils &);
    cMqttUtils& operator=(const cMqttUtils & );

    static cMqttUtils *instance(QObject *parent = nullptr);
    static void drop();

    void connectToServer();
    void disconnectToServer();
    bool isConnected();
    QMqttClient::ClientState getState();
private:
    QMqttClient *m_client = nullptr;
    cParseConfigureFile *m_ParseConfigureFile = nullptr;
    cConnectionInfo m_connectionInfo;
    static cMqttUtils *m_Instance;
    bool m_IsConnected = false;
signals:
    void sigConnectedToServer();
    void sigDisconnectedFromServer();
    void sigSendCommandToNode(QByteArray data);
public slots:
    void on_ConnectionStateChange();
    void on_ConnectionConnected();
    void on_ConnectionDisconnected();
    void on_ReceivedMessage(const QByteArray &message, const QMqttTopicName &topic);
    // Khi doc du lieu tu node sex goi slot nay de gui len server
    void on_PublicDataToServer(QByteArray data,  quint32 gwuid);
    void on_PublicKeepAlivePackage(quint32 gwuid);
    void on_NewGatewayFound(quint32 gwuid);

};

#endif // CMQTTUTILS_H
