#ifndef CSERIALPORTGATEWAY_H
#define CSERIALPORTGATEWAY_H

#include <QObject>
#include <QSerialPort>


class cSerialPortGateway : public QObject
{
    Q_OBJECT
public:
    explicit cSerialPortGateway(QObject *parent = nullptr);
    ~cSerialPortGateway();
    void setPortName(QString portName);
    void setBaudrate(quint16 baud);
    bool initSerialPort(QString portName, qint32 baud);
    void initSerialPort();
    void deinitSerialPort();
    qint32 getNumberOfPacket();
    QByteArray getOneDataPacket();
    bool forwardCommandFromServer(QByteArray command);
protected:
    void writeData(const QByteArray &data);
private:
    QSerialPort *m_SerialPort = nullptr;
    QString m_portName = nullptr;
    qint32 m_Baudrate = 0;

signals:
    void sendCommandToNode(QByteArray command);
public slots:
    void on_SendTCommandToNode(QByteArray command);
};

#endif // CSERIALPORTGATEWAY_H
