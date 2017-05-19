INCLUDEPATH += $$PWD
HEADERS += \
    $$PWD/excel_report.h
SOURCES += \
    $$PWD/excel_report.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/qtxlsx/release/ -lQtXlsx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/qtxlsx/debug/ -lQtXlsx
else:unix: LIBS += -L$$OUT_PWD/../../lib/qtxlsx/ -lQtXlsx

INCLUDEPATH += $$PWD/../../../lib/qtxlsx
DEPENDPATH += $$PWD/../../../lib/qtxlsx

#CONFIG(release, debug|release){
#    BUILD_TYPE=release
#    LIB_NAME=QtXlsx.dll
#}
#CONFIG(debug, debug|release){
#    BUILD_TYPE=debug
#    LIB_NAME=QtXlsxd.dll
#}

CONFIG(release, debug|release){BUILD_TYPE=release}
CONFIG(debug, debug|release){BUILD_TYPE=debug}

copydata.commands = $(COPY_FILE) \"$$shell_path($$clean_path($$OUT_PWD/../../lib/qtxlsx/$$BUILD_TYPE/QtXlsx.dll))\" \"$$shell_path($$OUT_PWD/$$BUILD_TYPE)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
