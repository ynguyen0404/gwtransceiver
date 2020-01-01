#include "cSerialWorker.h"

cSerialWorker *cSerialWorker::m_Instance = nullptr;


cSerialWorker::cSerialWorker(QObject *parent) : QObject(parent)
{
    m_Abort = false;
    m_Method = READ_NODE_DATA;
    m_CancelDelay = false;
}


cSerialWorker *cSerialWorker::instance(QObject *parent)
{
    static QMutex mutex;
    if (m_Instance == nullptr) {
        mutex.lock();
        m_Instance = new cSerialWorker(parent);
        mutex.unlock();
    }
    return m_Instance;
}

void cSerialWorker::drop()
{
    static QMutex mutex;
    mutex.lock();
    delete m_Instance;
    m_Instance = nullptr;
    mutex.unlock();
}


void cSerialWorker::abort()
{
    QMutexLocker locker(&m_Mutex);
    m_Abort = true;
}

void cSerialWorker::requestMethod(int m_method)
{
    QMutexLocker locker(&m_Mutex);
    m_Method = m_method;
    m_CancelDelay = true;
}

void cSerialWorker::setPollPeriod(qint32 sec)
{
    QMutexLocker locker(&m_Mutex);
    m_PollPeriod = static_cast<quint64>(sec * 1000);
}

void cSerialWorker::delay(quint64 ms)
{
    while (!m_CancelDelay && ms-- > 0) {
        QThread::msleep(1);
    }
    m_CancelDelay = false;
}

void cSerialWorker::mainLoop()
{
    forever {
        m_Mutex.lock();
        if (m_Abort)
        {
           emit finished();
           m_Mutex.unlock();
           return;
        }
        m_Mutex.unlock();
        switch (m_Method) {
        case READ_NODE_DATA:
            emit sigReadNodeData();
            break;
        case FORWARD_COMMAND:
            emit sigForwardCommand();
            m_Method = READ_NODE_DATA;
            break;
        default:
            break;
        }
        // Chia nho sleep ra der ngat khi co request Method
        delay(m_PollPeriod);
    }
}
