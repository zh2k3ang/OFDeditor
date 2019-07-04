#-------------------------------------------------
#
# Project created by QtCreator 2017-07-28T15:00:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#添加QtPrintSupport模块支持
qtHaveModule(printsupport): QT+=printsupport

TARGET = OFDeditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    informationextract.cpp \
    insertimagedialog.cpp \
    ofdprocess.cpp \
    paper.cpp \
    richtextprocess.cpp \
    pagesetupdialog.cpp \
    inserttabledialog.cpp \
    setlinedistancedialog.cpp \
    thread_getpagetext.cpp \
    thread_loadrichtext.cpp \
    countwordsdialog.cpp

HEADERS  += mainwindow.h \
    informationextract.h \
    insertimagedialog.h \
    ofdprocess.h \
    paper.h \
    richtextprocess.h \
    pagesetupdialog.h \
    inserttabledialog.h \
    setlinedistancedialog.h \
    thread_getpagetext.h \
    thread_loadrichtext.h \
    countwordsdialog.h

FORMS    += mainwindow.ui \
    insertimagedialog.ui \
    pagesetupdialog.ui \
    inserttabledialog.ui \
    setlinedistancedialog.ui \
    countwordsdialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    editor.rc
RC_FILE = editor.rc

