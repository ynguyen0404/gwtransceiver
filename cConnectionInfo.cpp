#include "cConnectionInfo.h"

cConnectionInfo::cConnectionInfo()
{

}

cConnectionInfo::cConnectionInfo(const cConnectionInfo &other)
:m_HostName(other.m_HostName),
  m_Port(other.m_Port),
  m_ClientId(other.m_ClientId),
  m_UserName(other.m_UserName),
  m_password(other.m_password),
  m_QoS(other.m_QoS),
  m_TopicPublicNoResponse(other.m_TopicPublicNoResponse),
  m_TopicSubscribeNoResponse(other.m_TopicSubscribeNoResponse),
  m_SerialPortName(other.m_SerialPortName),
  m_SerialPortBaud(other.m_SerialPortBaud)
{

}

cConnectionInfo::~cConnectionInfo()
{

}

QString cConnectionInfo::getHostName() const
{
    return m_HostName;
}

quint16 cConnectionInfo::getPort() const
{
    return m_Port;
}

QString cConnectionInfo::getClientId() const
{
    return m_ClientId;
}

QString cConnectionInfo::getUserName() const
{
    return m_UserName;
}

QString cConnectionInfo::getPassword() const
{
    return m_password;
}

quint8 cConnectionInfo::getQoS() const
{
    return m_QoS;
}

QString cConnectionInfo::getTopicPublicNoResponse() const
{
    return m_TopicPublicNoResponse;
}

QString cConnectionInfo::getTopicSubscribeNoResponse() const
{
    return m_TopicSubscribeNoResponse;
}

QString cConnectionInfo::getSerialPortName() const
{
    return m_SerialPortName;
}

qint32 cConnectionInfo::getSerialPortBaudrate() const
{
    return m_SerialPortBaud;
}

void cConnectionInfo::setHostName(QString hostname)
{
    m_HostName = hostname;
}

void cConnectionInfo::setPort(quint16 port)
{
    m_Port= port;
}

void cConnectionInfo::setClientId(QString clientId)
{
    m_ClientId= clientId;
}

void cConnectionInfo::setUserName(QString user)
{
    m_UserName = user;
}

void cConnectionInfo::setPassword(QString password)
{
    m_password = password;
}

void cConnectionInfo::setQoS(quint8 qos)
{
    m_QoS = qos;
}

void cConnectionInfo::setTopicPublicNoResponse(QString topic)
{
    m_TopicPublicNoResponse = topic;
}

void cConnectionInfo::setTopicSubscribeNoResponse(QString topic)
{
    m_TopicSubscribeNoResponse = topic;
}

void cConnectionInfo::setSerialPortName(QString portName)
{
    m_SerialPortName = portName;
}

void cConnectionInfo::setSerialPortBaudrate(quint32 baudrate)
{
    m_SerialPortBaud = baudrate;
}

