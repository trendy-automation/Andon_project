INCLUDEPATH += $$PWD
HEADERS += \
    $$PWD/excel_report.h
SOURCES += \
    $$PWD/excel_report.cpp

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../lib/qtxlsx/ -lqtxlsx
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../lib/qtxlsx/ -lqtxlsxd

INCLUDEPATH += $$PWD/../../../lib/qtxlsx
DEPENDPATH += $$PWD/../../../lib/qtxlsx
