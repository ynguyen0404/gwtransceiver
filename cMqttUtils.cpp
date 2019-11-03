#include "cMqttUtils.h"
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

cMqttUtils::cMqttUtils(QObject *parent) : QObject(parent)
{
    m_client = new QMqttClient();
    m_ParseConfigureFile = new cParseConfigureFile();
    m_connectionInfo = m_ParseConfigureFile->getConfigurationData();

    m_client->setHostname(m_connectionInfo.getHostName());
    m_client->setPort(m_connectionInfo.getPort());
    m_client->setClientId(m_connectionInfo.getClientId());
    m_client->setUsername(m_connectionInfo.getUserName());
    m_client->setPassword(m_connectionInfo.getPassword());
    m_client->setWillQoS(m_connectionInfo.getQoS());
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);

    connect(m_client, &QMqttClient::messageReceived, this, &cMqttUtils::on_ReceivedMessage);
    connect(m_client, &QMqttClient::stateChanged, this, &cMqttUtils::on_ConnectionStateChange);
    m_client->connectToHost();
}

cMqttUtils::~cMqttUtils()
{
    delete m_client;
}

void cMqttUtils::connectToServer()
{
    if (m_client != nullptr)
        m_client->connectToHost();
}

void cMqttUtils::disconnectToServer()
{
    if (m_client != nullptr)
        m_client->disconnectFromHost();
}

void cMqttUtils::on_ConnectionStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    qDebug() << "New State: " << content;
    if (m_client->state() == QMqttClient::Connected) {
        auto subscription = m_client->subscribe(QMqttTopicFilter(m_connectionInfo.getTopicSubscribeNoResponse()));
        qDebug() << "Subscription Response Code: " << subscription;
        if (!subscription) {
            qDebug("Could not subscribe. Is there a valid connection?");
            m_client->disconnectFromHost();
        }
    }
}

void cMqttUtils::on_ReceivedMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message;

    qDebug() << "Received Message From Server: " << content;
    //Parse JSON de lay Raw Command
    emit sendCommandToNode(message);
}

void cMqttUtils::on_PublicDataToServer(QByteArray data)
{
    Q_UNUSED(data);
    qDebug() << "Public Topic Name: " << m_connectionInfo.getTopicPublicNoResponse();
    QVariantMap exampleJson;
    QByteArray ba;
    ba.resize(5);
    ba[0] = 0x3c;
    ba[1] = 0xb8;
    ba[2] = 0x64;
    ba[3] = 0x18;
    ba[4] = 0xca;
    QJsonArray jsonArray;
    jsonArray.append(QJsonValue(ba[0]));
    jsonArray.append(QJsonValue(ba[1]));
    jsonArray.append(QJsonValue(ba[2]));
    QJsonObject itemObj;
    itemObj.insert("data", QJsonValue(jsonArray));
    QJsonDocument m_configInfoJson = QJsonDocument(itemObj);
    int val = m_client->publish(QMqttTopicName(m_connectionInfo.getTopicPublicNoResponse()), m_configInfoJson.toJson(), 2);
    qDebug() << "Public Return Val: " << val;
}

