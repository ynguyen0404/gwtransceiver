#include <QCoreApplication>
#include "cMqttUtils.h"
#include "cParseConfigureFile.h"
#include "cConnectionInfo.h"
#include "cDataUtils.h"
#include "cSerialPortGateway.h"
#include "cSerialWorker.h"
#include "cGatewayUID.h"
#include "cChirpstackMqtt.h"
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include <QThread>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WiredDevice>

void myMessageHandler(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    const QString homeFolder = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QFile outFile(homeFolder + "/" + "gwapplog.txt");

    if (outFile.exists()) {
        qint64 sizeInByte = outFile.size();
        if (sizeInByte > 10485760) {
            outFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        } else {
            outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        }
    } else {
         outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }


    QTextStream ts(&outFile);
    QString txt;
    switch (type) {
    case QtInfoMsg:
        txt = QString("Info: %1 %2 %3:").arg(context.file).arg(context.line).arg(context.function);
        ts << txt <<"\t"<< QString(msg)<<endl;
        break;
    case QtDebugMsg:
        txt = QString("Debug: %1 %2 %3:").arg(context.file).arg(context.line).arg(context.function);
        ts << txt <<"\t"<< QString(msg)<<endl;
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1 %2 %3:").arg(context.file).arg(context.line).arg(context.function);
        ts << txt <<"\t"<< QString(msg)<<endl;
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 %2 %3:").arg(context.file).arg(context.line).arg(context.function);
        ts << txt <<"\t"<< QString(msg)<<endl;
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 %2 %3:").arg(context.file).arg(context.line).arg(context.function);
        ts << txt <<"\t"<< QString(msg)<<endl;
        abort();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef LOG_TO_FILE
    qInstallMessageHandler(myMessageHandler);
#endif
    cParseConfigureFile m_parseConfigure;
    cMqttUtils *m_mqttUtils = nullptr;
    cChirpstackMqtt *m_ChirpstackMqtt = nullptr;
    NetworkManager::Device::List deviceList = NetworkManager::networkInterfaces();
    NetworkManager::WirelessDevice::Ptr wifiDevice;

    qDebug() << "Gateway UID: " << cGatewayUID::getGateWayUID();

//    // Set Default Value
//    m_connInfo.setHostName("103.232.120.72");
//    //m_connInfo.setHostName("tailor.cloudmqtt.com");
//    m_connInfo.setPort(30200);
//    // m_connInfo.setPort(17082);
//    m_connInfo.setClientId("paho2541357560754");
//    m_connInfo.setUserName("device@microbase.tech");
//    m_connInfo.setPassword("123456789");
//    //m_connInfo.setUserName("khlrlaie");
//    //m_connInfo.setPassword("oxSj4ZD4xqEI");
//    m_connInfo.setQoS(2);
//    m_connInfo.setTopicPublicNoResponse("/Oe8lpCDMj6tKh5DrBHvRonshuDQa/GATEWAY/data");
//    m_connInfo.setTopicSubscribeNoResponse("/Oe8lpCDMj6tKh5DrBHvRonshuDQa/GATEWAY/cmd");
//    m_connInfo.setSerialPortName("/dev/ttyACM0");
//    m_connInfo.setSerialPortBaudrate(static_cast<quint32>(115200));
//    m_parseConfigure.setConfigurationData(m_connInfo);
//    //End of Set Default Value

    m_mqttUtils = cMqttUtils::instance(&a);

    m_ChirpstackMqtt = cChirpstackMqtt::instance(&a);


    //Send Datetime to Node

    a.connect(m_ChirpstackMqtt, &cChirpstackMqtt::sigConnectedToServer, [] () {
        qDebug() << "Enqueue Local Date Time to Node";

    });

    a.connect(m_mqttUtils, &cMqttUtils::sigSendCommandToNode, [m_ChirpstackMqtt] (QByteArray dataToNode) {
        m_ChirpstackMqtt->on_SendDataToNode(dataToNode);
    });
//    m_SerialPortGw->on_SetDateTime();

    a.connect(m_ChirpstackMqtt, &cChirpstackMqtt::sigDataToVuServer, [m_mqttUtils] (QByteArray dataToServer) {
        if (m_mqttUtils->getState() == QMqttClient::Connected) {
            qDebug() << "Data Ready To Server";
            m_mqttUtils->on_PublicDataToServer(dataToServer);
        } else if (m_mqttUtils->getState() == QMqttClient::Connecting){
            qDebug() << "Connecting ...";
            qDebug() << "Data Lost: " << dataToServer;
        } else {
            qDebug() << "Connection Lost, Waiting For Network Connection....";
        }
    });

    a.connect(NetworkManager::notifier(), &NetworkManager::Notifier::primaryConnectionChanged, [m_mqttUtils, m_ChirpstackMqtt] (QString uni) {
        qDebug() << "primaryConnectionChanged: " << uni;
        qDebug() << "Disconnecting From Server...";
        m_mqttUtils->disconnectToServer();
        m_ChirpstackMqtt->disconnectToServer();
        if (QString::compare(uni, "/")) {
            qDebug() << "Connecting To Both Server...";
            m_mqttUtils->connectToServer();
            m_ChirpstackMqtt->connectToServer();
        }
    });

////    QObject::connect(m_mqttUtils, &cMqttUtils::sigSendCommandToNode, m_SerialPortGw, &cSerialPortGateway::on_ForwardCommandToNode);

////    a.connect(m_mqttUtils, &cMqttUtils::sigDisconnectedFromServer, [m_mqttUtils] () {
////        m_mqttUtils->connectToServer();;
////    });

    foreach (NetworkManager::Device::Ptr dev, deviceList)
    {
        qDebug() << "Found Devices: " << dev->type();
        if (dev->state() == NetworkManager::Device::State::Activated) {
            qDebug() << "Connecting To Local Chirpstack Server...";
            m_ChirpstackMqtt->connectToServer();
            m_mqttUtils->connectToServer();
            break;
        }
    }
    return a.exec();
}
