#include "cParseConfigureFile.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QTextStream>
#include <QDebug>

#define configureFileName               "/etc/default/gwtransceive.conf"

cParseConfigureFile::cParseConfigureFile(QObject *parent) : QObject(parent)
{

}

cParseConfigureFile::~cParseConfigureFile()
{

}

cConnectionInfo cParseConfigureFile::getConfigurationData()
{
    cConnectionInfo m_connInfo;
    QFile cfgFile(configureFileName);
    QStringList tmpList;
    m_connInfo.setHostName("103.232.120.72");
    m_connInfo.setPort(30200);
    m_connInfo.setClientId("paho2541357560754");
    m_connInfo.setUserName("device@microbase.tech");
    m_connInfo.setPassword("123456789");
    m_connInfo.setQoS(2);
    m_connInfo.setTopicPublicNoResponse("/Oe8lpCDMj6tKh5DrBHvRonshuDQa/GATEWAY/data");
    m_connInfo.setTopicSubscribeNoResponse("/Oe8lpCDMj6tKh5DrBHvRonshuDQa/GATEWAY/cmd");
    m_connInfo.setSerialPortName("/dev/ttymxc1");
    m_connInfo.setSerialPortBaudrate(static_cast<quint32>(115200));
    m_connInfo.setPollPeriod(5);
    m_connInfo.setGatewayUID("34f64bfffe9f234c");

    if (cfgFile.exists()) {
        if(cfgFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&cfgFile);
            QString content = in.readAll();
            cfgFile.close();
            if (!content.isEmpty()) {
                QJsonDocument itemDoc = QJsonDocument::fromJson(content.toLatin1());
                QJsonObject itemObj = itemDoc.object();
                m_connInfo.setHostName(itemObj.take("hostname").toString());
                m_connInfo.setPort(static_cast<quint16>(itemObj.take("port").toInt()));
                m_connInfo.setClientId(itemObj.take("clientid").toString());
                m_connInfo.setUserName(itemObj.take("username").toString());
                m_connInfo.setPassword(itemObj.take("password").toString());
                m_connInfo.setQoS(static_cast<quint8>(itemObj.take("qos").toInt()));
                m_connInfo.setTopicPublicNoResponse(itemObj.take("pubnoreptopic").toString());
                m_connInfo.setTopicSubscribeNoResponse(itemObj.take("subnoreptopic").toString());
                m_connInfo.setSerialPortName(itemObj.take("serialport").toString());
                m_connInfo.setSerialPortBaudrate(static_cast<qint32>(itemObj.take("serialportbaud").toInt()));
                m_connInfo.setPollPeriod(itemObj.take("pollperiod").toInt());
                m_connInfo.setGatewayUID(itemObj.take("gwid").toString());
                tmpList.clear();
                QJsonValue nodeidVal = itemObj.value("nodeuid");
                QJsonArray nodeidArray = nodeidVal.toArray();
                foreach (const QJsonValue & v, nodeidArray)
                        tmpList << v.toString();
                m_connInfo.setNodeID(tmpList);
                tmpList.clear();
                QJsonValue appidVal = itemObj.value("appid");
                QJsonArray appidArray = appidVal.toArray();
                foreach (const QJsonValue & v, appidArray)
                        tmpList << v.toString();
                m_connInfo.setAppID(tmpList);
            }
        }
    }
    return m_connInfo;
}

void cParseConfigureFile::setConfigurationData(cConnectionInfo conInfo)
{
    QVariantMap m_ConnfigInfoMap;
    QFile cfgFile(configureFileName);
    m_ConnfigInfoMap.insert("qos", conInfo.getQoS());
    m_ConnfigInfoMap.insert("hostname", conInfo.getHostName());
    m_ConnfigInfoMap.insert("port", conInfo.getPort());
    m_ConnfigInfoMap.insert("clientid", conInfo.getClientId());
    m_ConnfigInfoMap.insert("username", conInfo.getUserName());
    m_ConnfigInfoMap.insert("password", conInfo.getPassword());
    m_ConnfigInfoMap.insert("pubnoreptopic", conInfo.getTopicPublicNoResponse());
    m_ConnfigInfoMap.insert("subnoreptopic", conInfo.getTopicSubscribeNoResponse());
    m_ConnfigInfoMap.insert("serialport", conInfo.getSerialPortName());
    m_ConnfigInfoMap.insert("serialportbaud", conInfo.getSerialPortBaudrate());
    m_ConnfigInfoMap.insert("pollperiod", conInfo.getPollPeriod());
    m_ConnfigInfoMap.insert("gwid", conInfo.getGatewayUID());
    m_ConnfigInfoMap.insert("nodeuid", conInfo.getNodeIDs());
    m_ConnfigInfoMap.insert("appid", conInfo.getAppID());
    QJsonObject itemObj = QJsonObject::fromVariantMap(m_ConnfigInfoMap);
    QJsonDocument m_configInfoJson = QJsonDocument(itemObj);
    if(cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

        cfgFile.write(m_configInfoJson.toJson(QJsonDocument::Indented));
        cfgFile.close();
    }
}



