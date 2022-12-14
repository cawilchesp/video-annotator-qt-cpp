QT       += core gui help

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG -= app_bundle
CONFIG -= debug_and_release debug_and_release_target
CONFIG += static
TEMPLATE = app
TRANSLATIONS += ingles.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    framedisplay.cpp \
    helpwindow.cpp \
    calibracionwindow.cpp \
    annotationwindow.cpp \
    settingswindow.cpp \
    helpbrowser.cpp \
    labelwindow.cpp

HEADERS  += mainwindow.h \
    framedisplay.h \
    helpwindow.h \
    calibracionwindow.h \
    annotationwindow.h \
    settingswindow.h \
    helpbrowser.h \
    labelwindow.h

FORMS    += mainwindow.ui \
    helpwindow.ui \
    calibracionwindow.ui \
    annotationwindow.ui \
    settingswindow.ui \
    labelwindow.ui

INCLUDEPATH += C:\\opencv\\4.1.0\\include
CONFIG(debug, debug|release) {
    TARGET = TrafficVideoAnnotator_Debug
    win32 {
        LIBS += -LC:\\opencv\\4.1.0\\x86\\vc16\\lib \
            -lopencv_core410d \
            -lopencv_imgcodecs410d \
            -lopencv_imgproc410d \
            -lopencv_videoio410d \
    }
} else {
    TARGET = TrafficVideoAnnotator
    win32 {
        LIBS += -LC:\\opencv\\4.1.0\\x86\\vc16\\lib \
            -lopencv_core410 \
            -lopencv_imgcodecs410 \
            -lopencv_imgproc410 \
            -lopencv_videoio410 \
    }
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

RESOURCES += \
    recursos.qrc
