#ifndef CDATAUTILS_H
#define CDATAUTILS_H

#include <QObject>

class cDataUtils : public QObject
{
    Q_OBJECT
public:
    explicit cDataUtils(QObject *parent = nullptr);
    ~cDataUtils();
    enum ACKCODE {ACK = 0x06, NACK=0x15};
    static quint8 checkSum(QByteArray array);
    static bool isCheckSumCorrect(QByteArray array);
    static QByteArray commandNumOfDataExist();
    static QByteArray commandReadData();
    static QByteArray commandACK();
    static QByteArray commandSetDateTime();
};

#endif // CDATAUTILS_H
