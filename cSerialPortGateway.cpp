#include "cSerialPortGateway.h"
#include <QDebug>
#include <QThread>
#include "cDataUtils.h"

cSerialPortGateway::cSerialPortGateway(QObject *parent) : QObject(parent)
{

}

cSerialPortGateway::~cSerialPortGateway()
{

}

void cSerialPortGateway::setPortName(QString portName)
{
    m_portName = portName;
}

void cSerialPortGateway::setBaudrate(quint16 baud)
{
    m_Baudrate = baud;
}

bool cSerialPortGateway::initSerialPort(QString portName, qint32 baud)
{
    m_SerialPort = new QSerialPort();
    m_SerialPort->setPortName(portName);
    m_SerialPort->setBaudRate(baud);
    m_SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_SerialPort->setStopBits(QSerialPort::OneStop);
    m_SerialPort->setParity(QSerialPort::NoParity);
    m_SerialPort->setDataBits(QSerialPort::Data8);
    bool retVal = m_SerialPort->open(QIODevice::ReadWrite);
    if (retVal) {
        qDebug() << "Open Serial Port " << portName;
    } else {
        qDebug() << "Failed to open Serial Port " << portName;
    }
    return retVal;
}

void cSerialPortGateway::initSerialPort()
{
    m_SerialPort = new QSerialPort();
    m_SerialPort->setPortName(m_portName);
    m_SerialPort->setBaudRate(m_Baudrate);
    m_SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_SerialPort->setStopBits(QSerialPort::OneStop);
    m_SerialPort->setParity(QSerialPort::NoParity);
    m_SerialPort->setDataBits(QSerialPort::Data8);
    bool retVal = m_SerialPort->open(QIODevice::ReadWrite | QIODevice::Truncate);
    m_SerialPort->clear();
    if (retVal) {
        qDebug() << "Open Serial Port " << m_portName;
    } else {
        qDebug() << "Failed to open Serial Port " << m_Baudrate;
    }
}

void cSerialPortGateway::deinitSerialPort()
{
    if (m_SerialPort != nullptr)
        m_SerialPort->close();
}

qint32 cSerialPortGateway::getNumberOfPacket()
{
    qint32 retVal = 0;
    qint8 retry = 3;

    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        QByteArray cmdNumofPacket = cDataUtils::commandNumOfDataExist();
        do {
            writeData(cmdNumofPacket);
            if (m_SerialPort->waitForReadyRead(3000)) {
                QByteArray response = m_SerialPort->read(1);
                if (response.at(0) == cmdNumofPacket.at(0)) {
                    response.append(m_SerialPort->read(2));
                    int length = response.at(1);
                    length = length << 8 | response.at(2);
                    response.append(m_SerialPort->read(length + 1));
                    if (cDataUtils::isCheckSumCorrect(response)) {
                        qDebug() << "Received correct data";
                        retVal = response.at(3);
                        retVal = retVal << 8 | response.at(4);
                        retry = 0;
                    }
                }
            }
        } while(retry-- > 0);
    }
    return retVal;
}

QByteArray cSerialPortGateway::getOneDataPacket()
{
    QByteArray retVal;
    quint8 retry = 3;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        QByteArray cmdReadData = cDataUtils::commandReadData();
        do {
            writeData(cmdReadData);
            if (m_SerialPort->waitForReadyRead(3000)) {
                QByteArray response = m_SerialPort->read(1);
                if (response.at(0) == cmdReadData.at(0)) {
                    response.append(m_SerialPort->read(2));
                    int length = response.at(1);
                    length = length << 8 | response.at(2);
                    response.append(m_SerialPort->read(length + 1));
                    if (cDataUtils::isCheckSumCorrect(response)) {
                        qDebug() << "Received correct data";
                        retVal = response;
                        retry = 0;
                    }
                }
            }
        } while (retry-- > 0);
    }

    return retVal;retry = 0;
}

bool cSerialPortGateway::forwardCommandFromServer(QByteArray command)
{
    bool retVal = false;
    quint8 retry = 3;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        do {
            writeData(command);
            if (m_SerialPort->waitForReadyRead(3000)) {
                QByteArray response = m_SerialPort->read(1);
                if (response.at(0) == 0xFF) {
                    response.append(m_SerialPort->read(2));
                    int length = response.at(1);
                    length = length << 8 | response.at(2);
                    response.append(m_SerialPort->read(length + 1));
                    if (cDataUtils::isCheckSumCorrect(response)) {
                        qDebug() << "Received correct data";
                        if (response.at(3) == cDataUtils::ACK) {
                            retVal = true;
                            retry = 0;
                        }
                    }
                }
            }
        } while (retry-- > 0);
    }

    return retVal;
}

void cSerialPortGateway::writeData(const QByteArray &data)
{
    int retry = 3;
    bool writeDone = false;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        do {
            m_SerialPort->flush();
            m_SerialPort->write(data.data(), data.length());
            writeDone = m_SerialPort->waitForBytesWritten(1000);
        } while(retry-- > 0 && !writeDone);
    }
}

void cSerialPortGateway::on_SendTCommandToNode(QByteArray command)
{
    forwardCommandFromServer(command);
}

