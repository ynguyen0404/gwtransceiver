#include "cGatewayUID.h"
#include <QFile>

cGatewayUID::cGatewayUID(QObject *parent) : QObject(parent)
{

}

cGatewayUID::~cGatewayUID()
{

}

quint32 cGatewayUID::getGateWayUID()
{
    quint32 retVal = 0;
    QFile file("/sys/fsl_otp/HW_OCOTP_CFG1");
    if (file.open(QIODevice::ReadOnly)) {
        QString content = file.readAll();
        retVal = content.toUInt(nullptr, 16);
    }
    return retVal;
}
