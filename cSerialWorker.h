#ifndef CSERIALWORKER_H
#define CSERIALWORKER_H

#include <QObject>
#include <QThread>
#include <QMutex>

class cSerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit cSerialWorker(QObject *parent = nullptr);
    cSerialWorker (const cSerialWorker &);
    cSerialWorker& operator=(const cSerialWorker & );
    enum {READ_NODE_DATA, FORWARD_COMMAND};

    static cSerialWorker *instance(QObject *parent = nullptr);
    static void drop();
    void abort();
    void requestMethod(int m_method);
    void setPollPeriod(qint32 sec);
private:
    void delay(quint64 ms);
private:
    static cSerialWorker *m_Instance;
    bool m_Abort;
    int m_Method;
    quint64 m_PollPeriod = 5000;
    bool m_CancelDelay;
    QMutex m_Mutex;
signals:
    void finished();
    void sigReadNodeData();
    void sigForwardCommand();
    void sigSendDateTime();
public slots:
    void mainLoop();
};

#endif // CSERIALWORKER_H
