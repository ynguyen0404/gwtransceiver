#ifndef CSERIALPORTGATEWAY_H
#define CSERIALPORTGATEWAY_H

#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QMutex>


class cSerialPortGateway : public QObject
{
    Q_OBJECT
public:
    explicit cSerialPortGateway(QObject *parent = nullptr);
    ~cSerialPortGateway();

    static cSerialPortGateway *instance(QObject *parent = nullptr);
    static void drop();

    void setPortName(QString portName);
    void setBaudrate(quint16 baud);
    bool initSerialPort(QString portName, qint32 baud);
    void initSerialPort();
    void deinitSerialPort();
    void setNodeCommand(QByteArray command);
protected:
    void writeData(const QByteArray &data);
    QByteArray readData(bool *ok, const int timeout, const int length);
private:
    qint32 getNumberOfPacket();
    QByteArray getOneDataPacket();
    bool forwardCommandFromServer(QByteArray command);
private:
    QSerialPort *m_SerialPort = nullptr;

    QString m_portName = nullptr;
    qint32 m_Baudrate = 0;
    static cSerialPortGateway *m_Instance;
    QList<QByteArray> m_NodeCommandFromServer;
signals:
     void sigSendDataToServer(QByteArray data);
public slots:
    void on_ReadNodeData();
    void on_ForwardCommandToNode();
    void on_SetDateTime();
};

#endif // CSERIALPORTGATEWAY_H
