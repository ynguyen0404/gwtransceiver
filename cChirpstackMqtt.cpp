#include "cChirpstackMqtt.h"
#include <QMutex>
#include <QTimer>
#include <QDateTime>
#include <QHostAddress>
#include "cDataUtils.h"
#include "cJSONParser.h"

cChirpstackMqtt *cChirpstackMqtt::m_Instance = nullptr;

cChirpstackMqtt::cChirpstackMqtt(QObject *parent) : QObject(parent)
{
    m_client = new QMqttClient(parent);
    m_ParseConfigureFile = new cParseConfigureFile();
    m_connectionInfo = m_ParseConfigureFile->getConfigurationData();

    m_client->setHostname("127.0.0.1");
    m_client->setPort(1883);
    m_client->setKeepAlive(40);

    connect(m_client, &QMqttClient::messageReceived, this, &cChirpstackMqtt::on_ReceivedMessage);
    connect(m_client, &QMqttClient::stateChanged, this, &cChirpstackMqtt::on_ConnectionStateChange);
    connect(m_client, &QMqttClient::connected, this, &cChirpstackMqtt::on_ConnectionConnected);
    connect(m_client, &QMqttClient::disconnected, this, &cChirpstackMqtt::on_ConnectionDisconnected);

}

cChirpstackMqtt::~cChirpstackMqtt()
{
    delete m_client;
}

cChirpstackMqtt *cChirpstackMqtt::instance(QObject *parent)
{
    static QMutex mutex;
    if (m_Instance == nullptr) {
        mutex.lock();
        m_Instance = new cChirpstackMqtt(parent);
        mutex.unlock();
    }
    return m_Instance;
}

void cChirpstackMqtt::drop()
{
    static QMutex mutex;
    mutex.lock();
    delete m_Instance;
    m_Instance = nullptr;
    mutex.unlock();
}

void cChirpstackMqtt::connectToServer()
{
    if (m_client != nullptr) {
        QTimer::singleShot(700, m_client, SLOT(connectToHost()));
    }
}

void cChirpstackMqtt::disconnectToServer()
{
    if (m_client != nullptr)
    {
       m_client->disconnectFromHost();
    }
}

bool cChirpstackMqtt::isConnected()
{
    bool retVal = false;
    if (m_client->state() == QMqttClient::Connected)
        retVal = true;
    return retVal;
}


QMqttClient::ClientState cChirpstackMqtt::getState()
{
    return m_client->state();
}

void cChirpstackMqtt::on_ConnectionStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    qDebug() << "ChirpStack: " << "New State: " << content;
}

void cChirpstackMqtt::on_ConnectionConnected()
{
    QStringList m_AppID = m_connectionInfo.getAppID();
    QStringList m_nodeID = m_connectionInfo.getNodeIDs();
    QStringList subscribeTopics;
    for(int i = 0; i < m_AppID.count(); i++) {
        subscribeTopics << QString("application/%1/device/%2/rx").arg(m_AppID.at(i)).arg(m_nodeID.at(i));
    }
    foreach (QString topic, subscribeTopics) {
        auto subscription = m_client->subscribe(QMqttTopicFilter(topic), m_connectionInfo.getQoS());
        qDebug() << "Chirpstack:" << "Subscription Response Code: " << subscription;
        if (!subscription) {
            qDebug("Could not subscribe. Is there a valid connection?");
            m_client->disconnectFromHost();
            m_IsConnected = false;
            emit sigDisconnectedFromServer();
            break;
        } else {
            m_IsConnected = true;
        }
    }
    emit sigConnectedToServer();
}

void cChirpstackMqtt::on_ConnectionDisconnected()
{
    qDebug() << "Disconnected From Server, Client Error: " << m_client->error();
    emit sigDisconnectedFromServer();
}

void cChirpstackMqtt::on_ReceivedMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message;
    qDebug() << "Received Message From Server: " << content;
    QJsonDocument dataDoc = QJsonDocument::fromJson(message);
    QJsonObject dataObj = dataDoc.object();
    QByteArray rawData = QByteArray::fromBase64(dataObj.take("data").toString().toUtf8());
    qDebug() << "Data From Node: " << rawData;
    emit sigDataToVuServer(rawData);
}


void cChirpstackMqtt::on_SendDateTimeToNode()
{
    QByteArray datetimeCommand = cDataUtils::commandSetDateTime();
    QJsonDocument dataToSend = cJSONParser::createJSONToChirpstackServer(datetimeCommand);
    QStringList m_AppID = m_connectionInfo.getAppID();
    QStringList m_nodeID = m_connectionInfo.getNodeIDs();
    QStringList publicTopics;
    for(int i = 0; i < m_AppID.count(); i++) {
        publicTopics << QString("application/%1/device/%2/tx").arg(m_AppID.at(i)).arg(m_nodeID.at(i));
    }
    if (m_client->state() == QMqttClient::Connected) {
        foreach (QString topic, publicTopics) {
            qDebug() << "Public Topic Name: " << topic;
            qDebug() << "JSON To Server: " << dataToSend.toJson(QJsonDocument::Compact);
            int val = m_client->publish(QMqttTopicName(topic), dataToSend.toJson(QJsonDocument::Compact), 2);
            qDebug() << "Public Return Val: " << val;
        }
    }
}

void cChirpstackMqtt::on_SendDataToNode(QByteArray baseArray)
{
    QJsonDocument dataToSend = cJSONParser::createJSONToChirpstackServer(baseArray);
    QStringList m_AppID = m_connectionInfo.getAppID();
    QStringList m_nodeID = m_connectionInfo.getNodeIDs();
    QStringList publicTopics;
    for(int i = 0; i < m_AppID.count(); i++) {
        publicTopics << QString("application/%1/device/%2/tx").arg(m_AppID.at(i)).arg(m_nodeID.at(i));
    }
    if (m_client->state() == QMqttClient::Connected) {
        foreach (QString topic, publicTopics) {
            qDebug() << "Public Topic Name: " << topic;
            qDebug() << "JSON To Server: " << dataToSend.toJson(QJsonDocument::Compact);
            int val = m_client->publish(QMqttTopicName(topic), dataToSend.toJson(QJsonDocument::Compact), 2);
            qDebug() << "Public Return Val: " << val;
        }
    }
}
