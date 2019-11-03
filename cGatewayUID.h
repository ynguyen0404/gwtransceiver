#ifndef CGATEWAYUID_H
#define CGATEWAYUID_H

#include <QObject>

class cGatewayUID : public QObject
{
    Q_OBJECT
public:
    explicit cGatewayUID(QObject *parent = nullptr);
    ~cGatewayUID();
    static quint32 getGateWayUID();
};

#endif // CGATEWAYUID_H
