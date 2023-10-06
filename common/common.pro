QT -= gui
QT       += network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    auth.pb.cc \
    net_codec.cpp

HEADERS += \
    auth.pb.h \
    auth.proto \
    net_codec.h \
    common_def.h
    

INCLUDEPATH += ./3rd/include
#LIBS += -L./3rd/lib -lprotobuf

DESTDIR = ../common/output

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

