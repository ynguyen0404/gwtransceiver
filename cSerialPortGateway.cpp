#include "cSerialPortGateway.h"
#include <QDebug>

#include "cDataUtils.h"

cSerialPortGateway *cSerialPortGateway::m_Instance = nullptr;


cSerialPortGateway::cSerialPortGateway(QObject *parent) : QObject(parent)
{
    m_NodeCommandFromServer.clear();

}

cSerialPortGateway::~cSerialPortGateway()
{

}

cSerialPortGateway *cSerialPortGateway::instance(QObject *parent)
{
    static QMutex mutex;
    if (m_Instance == nullptr) {
        mutex.lock();
        m_Instance = new cSerialPortGateway(parent);
        mutex.unlock();
    }
    return m_Instance;
}

void cSerialPortGateway::drop()
{
    static QMutex mutex;
    mutex.lock();
    delete m_Instance;
    m_Instance = nullptr;
    mutex.unlock();
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
    m_SerialPort = new QSerialPort(this);
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
    m_SerialPort = new QSerialPort(this);
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

void cSerialPortGateway::setNodeCommand(QByteArray command)
{
    m_NodeCommandFromServer.append(command);
}

qint32 cSerialPortGateway::getNumberOfPacket()
{
    qint32 retVal = 0;
    qint8 retry = 3;
    QByteArray response;
    QByteArray tempArray;
    bool isReadDataOK;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        QByteArray cmdNumofPacket = cDataUtils::commandNumOfDataExist();
        do {
            response.clear();
            tempArray.clear();
            writeData(cmdNumofPacket);
            if (m_SerialPort->waitForReadyRead(3000)) {
                 tempArray = readData(&isReadDataOK, 2000, 1);
                 if (isReadDataOK) {
                     response.append(tempArray);
                     if (response.at(0) == cmdNumofPacket.at(0)) {
                         tempArray = readData(&isReadDataOK, 2000, 2);
                         if (isReadDataOK) {
                             response.append(tempArray);
                             int length = response.at(1);
                             length = length << 8 | response.at(2);
                             tempArray = readData(&isReadDataOK, 2000, length + 1);
                             if (isReadDataOK) {
                                 response.append(tempArray);
                                 if (cDataUtils::isCheckSumCorrect(response)) {
                                     qDebug() << "Get Num Package: Received correct data";
                                     retVal = response.at(3);
                                     retVal = retVal << 8 | response.at(4);
                                     retry = 0;
                                 } else {
                                     qDebug() << "Get Num Package: Incorrect Checksum";
                                 }
                             } else {
                                  qDebug() << "Get Num Package: Read Byte Failed, expected Length: " << length;
                                  qDebug() << "Data: " << m_SerialPort->readAll();
                             }
                         } else {
                             qDebug()  << "Get Num Package: Read 2 Byte Length Failed";
                         }
                     } else {
                         qDebug() << "Get Num Package: Start Byte Not Match: " <<  response;
                     }
                 } else {
                     qDebug() << "Get Num Package: Read Start Byte Failed";
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
    QByteArray tempArray;
    QByteArray response;
    bool isReadDataOK;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        QByteArray cmdReadData = cDataUtils::commandReadData();
        do {
            tempArray.clear();
            response.clear();
            writeData(cmdReadData);
            if (m_SerialPort->waitForReadyRead(3000)) {
                tempArray = readData(&isReadDataOK, 2000, 1);
                if (isReadDataOK) {
                    response.append(tempArray);
                    if (response.at(0) == cmdReadData.at(0)) {
                        tempArray = readData(&isReadDataOK, 2000, 2);
                        if (isReadDataOK) {
                            response.append(tempArray);
                            int length = response.at(1);
                            length = length << 8 | response.at(2);
                            tempArray = readData(&isReadDataOK, 5000, length + 1);
                            if (isReadDataOK) {
                                response.append(tempArray);
                                if (cDataUtils::isCheckSumCorrect(response)) {
                                    qDebug() << "Get One Data Package: Received correct data";
                                    retVal = response;
                                    retry = 0;
                                } else {
                                    qDebug() << "Get One Data Package: Incorrect Checksum";
                                }
                            } else {
                                qDebug() << "Get One Data Package: Read Byte Failed, expected Length: " << length;
                            }
                        } else {
                            qDebug()  << "Get One Data Package: Read 2 Byte Length Failed";
                        }
                    } else {
                        qDebug() << "Get One Data Package: Start Byte Not Match: " <<  response;
                    }
                } else {
                    qDebug() << "Get One Data Package: Read Start Byte Failed";
                }
            }
        } while (retry-- > 0);
    }

    return retVal;
}

bool cSerialPortGateway::forwardCommandFromServer(QByteArray command)
{
    bool retVal = false;
    quint8 retry = 3;
    QByteArray tempArray, response;
    bool isReadDataOK;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        QByteArray cmdACK = cDataUtils::commandACK();
        do {
            tempArray.clear();
            response.clear();
            writeData(command);
            if (m_SerialPort->waitForReadyRead(3000)) {
                tempArray = readData(&isReadDataOK, 2000, 1);
                if (isReadDataOK) {
                    response.append(tempArray);
                    if (response.at(0) == cmdACK.at(0)) {
                        tempArray = readData(&isReadDataOK, 2000, 2);
                        if (isReadDataOK) {
                            response.append(tempArray);
                            int length = response.at(1);
                            length = length << 8 | response.at(2);
                            tempArray = readData(&isReadDataOK, 5000, length + 1);
                            if (isReadDataOK) {
                                response.append(tempArray);
                                if (cDataUtils::isCheckSumCorrect(response)) {
                                    qDebug() << "Forward Command To Node: Receive Correct Data";
                                    int ack = response.at(3);
                                    if (ack == cDataUtils::ACK) {
                                        retVal = true;
                                        retry = 0;
                                    }
                                } else {
                                    qDebug() << "Get One Data Package: Incorrect Checksum";
                                }
                            } else {
                                qDebug() << "Forward Command To Node: Read Byte Failed, expected Length: " << length;
                            }
                        } else {
                            qDebug()  << "Forward Command To Node: Read 2 Byte Length Failed";
                        }
                    } else {
                        qDebug() << "Forward Command To Node: Start Byte Not Match: " <<  response;
                    }
                } else {
                    qDebug() << "Forward Command To Node: Read Start Byte Failed";
                }
            }
        } while (retry-- > 0);
    }

    return retVal;
}

void cSerialPortGateway::on_ReadNodeData()
{
    int numOfPackage = 0;
    QByteArray nodeDate;
    numOfPackage = getNumberOfPacket();
    qDebug() << "Num of Package: " << numOfPackage;
    for (int i = 0; i < numOfPackage; i++) {
        nodeDate = getOneDataPacket();
        if (cDataUtils::isCheckSumCorrect(nodeDate)) {
            qDebug() << "Node Data: " << nodeDate;
            emit sigSendDataToServer(nodeDate);
        }

    }
}

void cSerialPortGateway::on_ForwardCommandToNode()
{
    qDebug() << "Begin Forward Command To Node...";
    while (m_NodeCommandFromServer.count()) {
        bool retVal = forwardCommandFromServer(m_NodeCommandFromServer.takeFirst());
        qDebug() << "Forward Command To Node: " << retVal;
    }

}

void cSerialPortGateway::on_SetDateTime()
{
    QByteArray datetimeCommand = cDataUtils::commandSetDateTime();
    forwardCommandFromServer(datetimeCommand);
}

void cSerialPortGateway::writeData(const QByteArray &data)
{
    int retry = 3;
    bool writeDone = false;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        do {
            m_SerialPort->flush();
            m_SerialPort->clear(m_SerialPort->AllDirections);
            m_SerialPort->write(data.data(), data.length());
            writeDone = m_SerialPort->waitForBytesWritten(1000);
        } while(retry-- > 0 && !writeDone);
    }
}

QByteArray cSerialPortGateway::readData(bool *ok, const int timeout, const int length)
{
    QByteArray retVal;
    QByteArray tempVal;
    *ok = false;
    int count = 10;
    int timeDelay = timeout / count;
    int tempLength = length;
    if(m_SerialPort != nullptr && m_SerialPort->isOpen()) {
        while (m_SerialPort->waitForReadyRead(timeDelay) || count-- > 0) {
            tempVal = m_SerialPort->read(tempLength);
            retVal.append(tempVal);

            if (retVal.count() == length) {
                *ok = true;
                break;
            } else {
                tempLength -= tempVal.count();
            }
        }
    }
    return retVal;
}


