QT       += core

#include($${ANDNPRJ_LIBPATH}/SmtpClient/SmtpClient.pri)

# Location of SMTP Library
SMTP_LIBRARY_LOCATION = $${ANDNPRJ_LIBPATH}/SmtpClient/

win32:CONFIG(release, debug|release): LIBS += -L$$SMTP_LIBRARY_LOCATION/release/ -lSMTPEmail
else:win32:CONFIG(debug, debug|release): LIBS += -L$$SMTP_LIBRARY_LOCATION/debug/ -lSMTPEmail
else:unix: LIBS += -L$$SMTP_LIBRARY_LOCATION -lSMTPEmail

#INCLUDEPATH += $$SMTP_LIBRARY_LOCATION
#DEPENDPATH += $$SMTP_LIBRARY_LOCATION

INCLUDEPATH += $${ANDNPRJ_LIBPATH}/SmtpClient/src

INCLUDEPATH+=$$PWD

HEADERS += $$PWD/sendemail.h
SOURCES += $$PWD/sendemail.cpp


