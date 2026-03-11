QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/blob.cpp \
    src/camera.cpp \
    src/gamewidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/shaderprogram.cpp

HEADERS += \
    include/blob.h \
    include/gamewidget.h \
    include/mainwindow.h \
    include/shaderprogram.h \
    include/camera.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    shaders/arrowShader.frag \
    shaders/arrowShader.vert \
    shaders/basicShader.frag \
    shaders/basicShader.vert \
    shaders/pointShader.frag \
    shaders/pointShader.vert
