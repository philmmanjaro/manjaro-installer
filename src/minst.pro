#-------------------------------------------------
#
# Project created by QtCreator 2012-11-21T11:39:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lmawiz -lparted -lmhwd -lX11
INCLUDEPATH += "/usr/include/parted"

TARGET = minst
TEMPLATE = app


SOURCES += main.cpp\
        minst.cpp \
    pages/page_intro.cpp \
    global.cpp \
    widgets/timezonewidget.cpp \
    pages/page_timezone.cpp \
    pages/page_managepartitions.cpp \
    partitionmanager/device.cpp \
    partitionmanager/partition.cpp \
    partitionmanager/filesystems/filesystem.cpp \
    partitionmanager/filesystems/ext4.cpp \
    partitionmanager/utils.cpp \
    partitionmanager/mparted_core.cpp \
    partitionmanager/operations/operation.cpp \
    partitionmanager/unallocatedutils.cpp \
    partitionmanager/operations/operationremove.cpp \
    partitionmanager/mparted_virtual.cpp \
    partitionmanager/operations/operationcreate.cpp \
    widgets/partitioncreatedialog.cpp \
    widgets/partresizerwidget.cpp \
    pages/page_gpudriver.cpp \
    pages/page_keyboard.cpp \
    widgets/partwidget.cpp \
    partitionmanager/operations/operationresize.cpp \
    partitionmanager/operations/operationformat.cpp \
    widgets/keyboardpreview.cpp

HEADERS  += minst.h \
    pages/page_intro.h \
    const.h \
    global.h \
    widgets/timezonewidget.h \
    pages/page_timezone.h \
    pages/page_managepartitions.h \
    partitionmanager/device.h \
    partitionmanager/partition.h \
    partitionmanager/const.h \
    partitionmanager/filesystems/filesystem.h \
    partitionmanager/filesystems/ext4.h \
    partitionmanager/utils.h \
    partitionmanager/mparted_core.h \
    partitionmanager/mparted.h \
    partitionmanager/operations/operation.h \
    partitionmanager/unallocatedutils.h \
    partitionmanager/operations/operationremove.h \
    partitionmanager/mparted_virtual.h \
    partitionmanager/operations/operationcreate.h \
    widgets/partitioncreatedialog.h \
    widgets/partresizerwidget.h \
    pages/page_gpudriver.h \
    pages/page_keyboard.h \
    widgets/partwidget.h \
    partitionmanager/operations/operationresize.h \
    partitionmanager/operations/operationformat.h \
    widgets/keyboardpreview.h


FORMS += \
    pages/page_intro.ui \
    pages/page_timezone.ui \
    pages/page_managepartitions.ui \
    widgets/partitioncreatedialog.ui \
    pages/page_gpudriver.ui \
    pages/page_keyboard.ui

RESOURCES += \
    minst_resources.qrc
