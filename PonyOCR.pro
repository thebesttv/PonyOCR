TEMPLATE = app
TARGET = PonyOCR
INCLUDEPATH += .

QT += core gui widgets network \
      webenginewidgets webchannel

unix : !macx {  # in unix but not macOS
    QT += dbus
}

# set icon
RC_ICONS = icon.ico

# from WebEngineWidgets/MarkdownEditor example
# Disable Qt Quick compiler because the example doesn't use QML, but more importantly so that
# the source code of the .js files is not removed from the embedded qrc file.
CONFIG -= qtquickcompiler

HEADERS += ocrrequest.h \
           ponyocr.h

SOURCES += main.cpp \
           ponyocr.cpp

include(Capture/capture.pri)
include(API/api.pri)
include(Markdown/markdown.pri)
include(Configuation/configuation.pri)

RESOURCES += resources.qrc

TRANSLATIONS += pony_zh.ts
