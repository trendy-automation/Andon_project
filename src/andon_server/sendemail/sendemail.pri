QT       += core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/SmtpClient/release/ -lSMTPEmail
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/SmtpClient/debug/ -lSMTPEmail
else:unix: LIBS += -L$$OUT_PWD/../../lib/SmtpClient/ -lSMTPEmail

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/sendemail.h
SOURCES += $$PWD/sendemail.cpp

CONFIG(release, debug|release){BUILD_TYPE=release}
CONFIG(debug, debug|release){BUILD_TYPE=debug}

copydata.commands = $(COPY_FILE) \"$$shell_path($$clean_path($$OUT_PWD/../../lib/SmtpClient/$$BUILD_TYPE/SMTPEmail.dll))\" \"$$shell_path($$OUT_PWD/$$BUILD_TYPE)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
