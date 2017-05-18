TEMPLATE = lib
include(snap7.pri)
#LIBS += -lsnap7
TARGET = snap7
QT += core network


#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../release/ -lsnap7
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../debug/ -lsnap7
#else:unix: LIBS += -L$$OUT_PWD/./ -lsnap7

#INCLUDEPATH += $$PWD
#DEPENDPATH += $$PWD
