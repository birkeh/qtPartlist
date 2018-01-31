#-------------------------------------------------
#
# Project created by QtCreator 2017-12-28T09:29:37
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(3rdparty/qtxlsx/src/xlsx/qtxlsx.pri)

TARGET = qtPartlist
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = icon.ico

SOURCES += \
        main.cpp \
        cmainwindow.cpp \
    cpartlistwindow.cpp \
    common.cpp \
    cdistributorwindow.cpp \
    cpartwindow.cpp \
    cdistributor.cpp \
    cdistributoreditdialog.cpp \
    cpartgroup.cpp \
    cpart.cpp \
    cparteditdialog.cpp \
    cpartdistributor.cpp \
    clabel.cpp \
    cpartdistributoreditdialog.cpp \
    cpartlistitem.cpp \
    cpartlistitemdelegate.cpp \
    cpartlistitemeditdialog.cpp \
    cinputlistdialog.cpp \
    cpartlistselectdialog.cpp \
    cpartlistlistdelegate.cpp

HEADERS += \
        cmainwindow.h \
    cpartlistwindow.h \
    common.h \
    cdistributorwindow.h \
    cpartwindow.h \
    cdistributor.h \
    cdistributoreditdialog.h \
    cpartgroup.h \
    cpart.h \
    cparteditdialog.h \
    cpartdistributor.h \
    clabel.h \
    cpartdistributoreditdialog.h \
    cpartlistitem.h \
    cpartlistitemdelegate.h \
    cpartlistitemeditdialog.h \
    cinputlistdialog.h \
    cpartlistselectdialog.h \
    cpartlistlistdelegate.h

FORMS += \
        cmainwindow.ui \
    cpartlistwindow.ui \
    cdistributorwindow.ui \
    cpartwindow.ui \
    cdistributoreditdialog.ui \
    cparteditdialog.ui \
    cpartdistributoreditdialog.ui \
    cpartlistitemeditdialog.ui \
    cinputlistdialog.ui \
    cpartlistselectdialog.ui

RESOURCES += \
    qtpartlist.qrc

DISTFILES += \
    icon.png \
    icon.ico
