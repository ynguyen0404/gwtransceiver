QT -= gui

QT += mqtt serialport network dbus

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cChirpstackMqtt.cpp \
        cConnectionInfo.cpp \
        cDataUtils.cpp \
        cGatewayUID.cpp \
        cJSONParser.cpp \
        cMqttUtils.cpp \
        cParseConfigureFile.cpp \
        cSerialPortGateway.cpp \
        cSerialWorker.cpp \
        main.cpp

HEADERS += \
    cChirpstackMqtt.h \
    cConnectionInfo.h \
    cDataUtils.h \
    cGatewayUID.h \
    cJSONParser.h \
    cMqttUtils.h \
    cParseConfigureFile.h \
    cSerialPortGateway.h \
    cSerialWorker.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!android: config.path = /etc/default
config.files = data/gwtransceive.conf
!isEmpty(config.path): INSTALLS += config

unix:!android: service.path = /lib/systemd/system
service.files = data/gwtransceiver.service
!isEmpty(service.path): INSTALLS += service
