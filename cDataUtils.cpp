#include "cDataUtils.h"
#include <QDateTime>

cDataUtils::cDataUtils(QObject *parent) : QObject(parent)
{

}

cDataUtils::~cDataUtils()
{

}

quint8 cDataUtils::checkSum(QByteArray array)
{
    quint32 sum = 0;
    quint8 retVal = 0;
    for (int i = 0; i < array.length(); i++) {
        sum += static_cast<quint8>(array.at(i));
    }
    retVal = static_cast<quint8>(sum&0xFF);
    return retVal;
}

bool cDataUtils::isCheckSumCorrect(QByteArray array)
{
    bool retVal = false;
    QByteArray tempArary = array;
    if (tempArary.length() > 2) {
        quint8 checksum = tempArary.at(tempArary.length() - 1);
        tempArary.remove(tempArary.length() - 1, 1);
        if (checkSum(tempArary) == checksum) {
            retVal = true;
        }
    }
    return retVal;
}

QByteArray cDataUtils::commandNumOfDataExist()
{
    QByteArray retVal;
    retVal.append(static_cast<char>(0x02));
    retVal.append(static_cast<char>(0x00));
    retVal.append(static_cast<char>(0x00));
    retVal.append(static_cast<char>(checkSum(retVal)));
    return retVal;
}

QByteArray cDataUtils::commandReadData()
{
    QByteArray retVal;
    retVal.append(static_cast<char>(0x01));
    retVal.append(static_cast<char>(0x00));
    retVal.append(static_cast<char>(0x00));
    retVal.append(static_cast<char>(checkSum(retVal)));
    return retVal;
}

QByteArray cDataUtils::commandACK()
{
    QByteArray retVal;
    retVal.append(static_cast<char>(0xFF));
    retVal.append(static_cast<char>(0x00));
    retVal.append(static_cast<char>(0x01));
    retVal.append(static_cast<char>(0x06));
    retVal.append(static_cast<char>(checkSum(retVal)));
    return retVal;
}

QByteArray cDataUtils::commandSetDateTime()
{
    QByteArray retVal;
    QDateTime currentTime = QDateTime::currentDateTime();
    retVal.append(static_cast<char>(0x83));
    retVal.append(static_cast<char>(0x00));
    retVal.append(static_cast<char>(0x06));
    // yyMMddhhmmss
    retVal.append(static_cast<char>(currentTime.date().year() % 100));
    retVal.append(static_cast<char>(currentTime.date().month()));
    retVal.append(static_cast<char>(currentTime.date().day()));
    retVal.append(static_cast<char>(currentTime.time().hour()));
    retVal.append(static_cast<char>(currentTime.time().minute()));
    retVal.append(static_cast<char>(currentTime.time().second()));
    retVal.append(static_cast<char>(checkSum(retVal)));
    return retVal;
}
