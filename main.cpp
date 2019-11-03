#include <QCoreApplication>
#include "cMqttUtils.h"
#include "cParseConfigureFile.h"
#include "cConnectionInfo.h"
#include "cDataUtils.h"
#include "cSerialPortGateway.h"
#include "cGatewayUID.h"
#include <QStandardPaths>
#include <QFile>
#include <QDebug>

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
    cConnectionInfo m_connInfo;
    cMqttUtils *m_mqttUtils = nullptr;
    cSerialPortGateway *m_SerialPortGw = nullptr;

    qDebug() << "Gateway UID: " << cGatewayUID::getGateWayUID();

    // Set Default Value
    m_connInfo.setHostName("103.232.120.72");
    m_connInfo.setPort(30200);
    m_connInfo.setClientId("paho2541357560754");
    m_connInfo.setUserName("device@microbase.tech");
    m_connInfo.setPassword("123456789");
    m_connInfo.setQoS(2);
    m_connInfo.setTopicPublicNoResponse("/Oe8lpCDMj6tKh5DrBHvRonshuDQa/GATEWAY/data");
    m_connInfo.setTopicSubscribeNoResponse("/Oe8lpCDMj6tKh5DrBHvRonshuDQa/GATEWAY/cmd");
    m_connInfo.setSerialPortName("/dev/ttymxc0");
    m_connInfo.setSerialPortBaudrate(static_cast<quint32>(115200));
    m_parseConfigure.setConfigurationData(m_connInfo);
    //End of Set Default Value

    m_connInfo = m_parseConfigure.getConfigurationData();


    m_SerialPortGw = new cSerialPortGateway();
    if (m_SerialPortGw->initSerialPort(m_connInfo.getSerialPortName(), m_connInfo.getSerialPortBaudrate())) {
        m_mqttUtils = new cMqttUtils();
        QObject::connect(m_mqttUtils, &cMqttUtils::sendCommandToNode, m_SerialPortGw, &cSerialPortGateway::on_SendTCommandToNode);
    }


    //qint32 numpackage = m_SerialPortGw->getNumberOfPacket();
    //QByteArray onePacket = m_SerialPortGw->getOneDataPacket();
    //QByteArray exampleCmd = QByteArray::fromRawData("\x82\x00\x00\x82", 4);
    //bool retVal = m_SerialPortGw->forwardCommandFromServer(exampleCmd);
    //qDebug() << "No Package: " << retVal;
    //m_mqttUtils = new cMqttUtils();

    return a.exec();
}
