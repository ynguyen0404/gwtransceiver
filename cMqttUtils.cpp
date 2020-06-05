#include "cMqttUtils.h"
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>
#include "cJSONParser.h"


cMqttUtils *cMqttUtils::m_Instance = nullptr;


cMqttUtils::cMqttUtils(QObject *parent) : QObject(parent)
{
    m_client = new QMqttClient(parent);
    m_ParseConfigureFile = new cParseConfigureFile();
    m_connectionInfo = m_ParseConfigureFile->getConfigurationData();

    m_client->setHostname(m_connectionInfo.getHostName());
    m_client->setPort(m_connectionInfo.getPort());
    m_client->setClientId(m_connectionInfo.getClientId());
    m_client->setUsername(m_connectionInfo.getUserName());
    m_client->setPassword(m_connectionInfo.getPassword());
    m_client->setWillQoS(m_connectionInfo.getQoS());
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    m_client->setKeepAlive(40);
    connect(m_client, &QMqttClient::messageReceived, this, &cMqttUtils::on_ReceivedMessage);
    connect(m_client, &QMqttClient::stateChanged, this, &cMqttUtils::on_ConnectionStateChange);
    connect(m_client, &QMqttClient::connected, this, &cMqttUtils::on_ConnectionConnected);
    connect(m_client, &QMqttClient::disconnected, this, &cMqttUtils::on_ConnectionDisconnected);
}

cMqttUtils::~cMqttUtils()
{
    delete m_client;
}

cMqttUtils *cMqttUtils::instance(QObject *parent)
{
    static QMutex mutex;
    if (m_Instance == nullptr) {
        mutex.lock();
        m_Instance = new cMqttUtils(parent);
        mutex.unlock();
    }
    return m_Instance;
}


void cMqttUtils::drop()
{
    static QMutex mutex;
    mutex.lock();
    delete m_Instance;
    m_Instance = nullptr;
    mutex.unlock();
}

void cMqttUtils::connectToServer()
{
    if (m_client != nullptr) {
        QTimer::singleShot(500, m_client, SLOT(connectToHost()));
    }
}

void cMqttUtils::disconnectToServer()
{
    if (m_client != nullptr)
    {
       m_client->disconnectFromHost();
    }
}

bool cMqttUtils::isConnected()
{
    bool retVal = false;
    if (m_client->state() == QMqttClient::Connected)
        retVal = true;
    return retVal;
}

QMqttClient::ClientState cMqttUtils::getState()
{
    return m_client->state();
}

void cMqttUtils::on_ConnectionStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    qDebug() << "New State: " << content;
}

void cMqttUtils::on_ConnectionConnected()
{
    auto subscription = m_client->subscribe(QMqttTopicFilter(m_connectionInfo.getTopicSubscribeNoResponse()), m_connectionInfo.getQoS());
    qDebug() << "cMQtt: " << "Subscription Response Code: " << subscription;
    if (!subscription) {
        qDebug("Could not subscribe. Is there a valid connection?");
        m_client->disconnectFromHost();
    } else {
        m_IsConnected = true;
    }
    emit sigConnectedToServer();
}

void cMqttUtils::on_ConnectionDisconnected()
{
    qDebug() << "Disconnected From Server, Client Error: " << m_client->error();
    emit sigDisconnectedFromServer();
}

void cMqttUtils::on_ReceivedMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message;
    qDebug() << "Received Message From Server: " << content;
    QByteArray serverCommand;
    serverCommand = cJSONParser::rawCommandFromServer(message);
    qDebug() << "Command To Node: " << serverCommand;
    emit sigSendCommandToNode(serverCommand);
}

void cMqttUtils::on_PublicDataToServer(QByteArray data)
{
    QJsonDocument dataToSend = cJSONParser::createJSONToServer(data);
    if (m_client->state() == QMqttClient::Connected) {
        qDebug() << "Public Topic Name: " << m_connectionInfo.getTopicPublicNoResponse();
        qDebug() << "JSON To Server: " << dataToSend.toJson(QJsonDocument::Compact);
        int val = m_client->publish(QMqttTopicName(m_connectionInfo.getTopicPublicNoResponse()), dataToSend.toJson(QJsonDocument::Compact), 2);
        qDebug() << "Public Return Val: " << val;
    }
}

