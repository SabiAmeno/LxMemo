QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LxMemo/main.cpp \
    LxMemo/AnimationSW.cpp \
    LxMemo/ColorButton.cpp \
    LxMemo/ColorChunk.cpp \
    LxMemo/ColorLabel.cpp \
    LxMemo/ColorSelector.cpp \
    LxMemo/db.cpp \
    LxMemo/EditTools.cpp \
    LxMemo/ListItem.cpp \
    LxMemo/LxMemo.cpp \
    LxMemo/Memo.cpp \
    LxMemo/MemoDialog.cpp \
    LxMemo/Searcher.cpp \
    LxMemo/SizeSelector.cpp \
    LxMemo/TxtDelegate.cpp \
    LxMemo/lucencydialog.cpp \
    LxMemo/MessageBox.cpp \
    LxMemo/FolderCreator.cpp \
    LxMemo/Folder.cpp \
    LxMemo/MessageShower.cpp\
    LxMemo/Meta.cpp \
    LxMemo/Util.cpp \
    LxMemo/sqlite3.c

HEADERS += \
    LxMemo/AnimationSW.h \
    LxMemo/ColorButton.h \
    LxMemo/ColorChunk.h \
    LxMemo/ColorLabel.h \
    LxMemo/ColorSelector.h \
    LxMemo/db.h \
    LxMemo/EditTools.h \
    LxMemo/ListItem.h \
    LxMemo/LxMemo.h \
    LxMemo/Memo.h \
    LxMemo/MemoDialog.h \
    LxMemo/Searcher.h \
    LxMemo/SizeSelector.h \
    LxMemo/TxtDelegate.h \
    LxMemo/resource.h \
    LxMemo/lucencydialog.h \
    LxMemo/MessageBox.h \
    LxMemo/FolderCreator.h \
    LxMemo/Folder.h\
    LxMemo/MessageShower.h \
    LxMemo/Meta.h \
    LxMemo/Util.h \
    LxMemo/sqlite3.h


FORMS += \
    LxMemo/ColorSelector.ui \
    LxMemo/EditTools.ui \
    LxMemo/lucencydialog.ui \
    LxMemo/LxMemo.ui \
    LxMemo/MemoDialog.ui  \
    LxMemo/SizeSelector.ui \
    LxMemo/MessageBox.ui \
    LxMemo/FolderCreator.ui

#INCLUDEPATH += 3d/db/inc

RESOURCES += \
    LxMemo/LxMemo.qrc \
    LxMemo/LxMemo.rc

LIBS += -luser32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
