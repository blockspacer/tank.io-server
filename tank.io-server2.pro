QT       += core
QT       -= gui
QT       += sql



TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
#CONFIG -= qt
DEFINES  += SERVER_SIDE

CONFIG += c++11
INCLUDEPATH += ../../Classes
#INCLUDEPATH += /usr/local/boost_1_67_0/include/
#LIBS += -L"/usr/local/boost_1_67_0/"
LIBS +=  -lboost_thread -lboost_system   -lrt

INCLUDEPATH += ../../Classes/ImportantCommon
SOURCES += \
    server_delegator.cpp \
    Classes/MainServer.cpp \
    Engine/BaseServer.cpp \
    DB/DBAccess.cpp \
    base/asyncmap.cpp \
    base/AsyncCyirCularBuffer.cpp \
    base/BoundedArray.cpp \
    Engine/SharedMemoryLoger.cpp \
    Engine/Logger.cpp \
    Classes/Room.cpp \
    Engine/BaseRoom.cpp \
    debuge_tools/DebugeTools.cpp \
    MathShapeUtils/Shape.cpp \
#    ../../Classes/cocos2d_server/CocoBase.cpp \
    ../../Classes/GameCore/BoardObject.cpp \
    ../../Classes/GameCore/GameCore.cpp \
    ../../Classes/GameCore/Tank.cpp \
    ../../Classes/GameCore/Misle.cpp \
    ../../Classes/GameCore/PlatformLine.cpp \
    ../../Classes/base_message/BaseMessage.cpp \
    ../../Classes/game_message/Messages.cpp \
    ../../Classes/GameCore/AbstractTankViewHandler.cpp \
    ../../Classes/GameCore/BoardObjectEvent.cpp

HEADERS += \
    Classes/MainServer.h \
    Engine/BaseServer.h \
    DB/DBAccess.h \
    base/asyncmap.h \
    base/AsyncCyirCularBuffer.h \
    base/BoundedArray.h \
    base/rbtree.h \
    Engine/SharedMemoryLoger.h \
    Engine/Logger.h \
    Classes/Room.h \
    Engine/BaseRoom.h \
    debuge_tools/DebugeTools.h \
    MathShapeUtils/Shape.h \
    ../../Classes/cocos2d_server/CocoBase.h \
    ../../Classes/GameCore/BoardObject.h \
    ../../Classes/GameCore/GameCore.h \
    ../../Classes/GameCore/Tank.h \
    ../../Classes/GameCore/Misle.h \
    ../../Classes/GameCore/PlatformLine.h \
    ../../Classes/cocos2d_server/cocos2d_server.h \
    ../../Classes/base_message/BaseMessage.h \
    ../../Classes/game_message/Messages.h \
    ../../Classes/game_message/societalgaming.h \
    ../../Classes/game_message/chat.h \
    ../../Classes/GameCore/AbstractTankViewHandler.h \
    ../../Classes/GameCore/BoardObjectEvent.h

