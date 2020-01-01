#ifndef CPARSECONFIGUREFILE_H
#define CPARSECONFIGUREFILE_H

#include <QObject>
#include "cConnectionInfo.h"

class cParseConfigureFile : public QObject
{
    Q_OBJECT
public:
    explicit cParseConfigureFile(QObject *parent = nullptr);
    ~cParseConfigureFile();
    cConnectionInfo getConfigurationData();
    void setConfigurationData(cConnectionInfo conInfo);
};

#endif // CPARSECONFIGUREFILE_H
