QT       += core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/SmtpClient/release/ -lSMTPEmail
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/SmtpClient/debug/ -lSMTPEmail
else:unix: LIBS += -L$$OUT_PWD/../../lib/SmtpClient/ -lSMTPEmail

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/sendemail.h
SOURCES += $$PWD/sendemail.cpp


