#ifndef CCHIRPSTACKMQTT_H
#define CCHIRPSTACKMQTT_H

#include <QObject>
#include <QThread>
#include <QMqttClient>
#include "cConnectionInfo.h"
#include "cParseConfigureFile.h"

class cChirpstackMqtt : public QObject
{
    Q_OBJECT
public:
    explicit cChirpstackMqtt(QObject *parent = nullptr);
    ~cChirpstackMqtt();

    cChirpstackMqtt (const cChirpstackMqtt &);
    cChirpstackMqtt& operator=(const cChirpstackMqtt & );
    static cChirpstackMqtt *instance(QObject *parent = nullptr);
    static void drop();

    void connectToServer();
    void disconnectToServer();
    bool isConnected();
    QMqttClient::ClientState getState();


private:
    QMqttClient *m_client = nullptr;
    static cChirpstackMqtt *m_Instance;
    bool m_isConnected = false;
    cParseConfigureFile *m_ParseConfigureFile = nullptr;
    cConnectionInfo m_connectionInfo;
    bool m_IsConnected = false;
    QList<quint32> gwUIDList;
signals:
    void sigConnectedToServer();
    void sigDisconnectedFromServer();
    void sigDataToVuServer(QByteArray data, quint32 gwuid);
    void sigNewGatewayFound(quint32 gwuid);

public slots:
    void on_ConnectionStateChange();
    void on_ConnectionConnected();
    void on_ConnectionDisconnected();
    void on_ReceivedMessage(const QByteArray &message, const QMqttTopicName &topic);
    // Khi doc du lieu tu node sex goi slot nay de gui len server
    void on_SendDateTimeToNode();
    void on_SendDataToNode(QByteArray baseArray);
};

#endif // CCHIRPSTACKMQTT_H
