QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    addfriend.cpp \
    addgroup.cpp \
    chat_widget.cpp \
    creategroup.cpp \
    groupchat.cpp \
    main.cpp \
    privatechat.cpp \
    recv_file_thread.cpp \
    send_file_thread.cpp \
    widget.cpp

HEADERS += \
    addfriend.h \
    addgroup.h \
    chat_widget.h \
    creategroup.h \
    groupchat.h \
    privatechat.h \
    recv_file_thread.h \
    send_file_thread.h \
    widget.h

FORMS += \
    addfriend.ui \
    addgroup.ui \
    chat_widget.ui \
    creategroup.ui \
    groupchat.ui \
    privatechat.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myPix.qrc
