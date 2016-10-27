QT       += core gui
QT       += widgets
QT       += multimedia

SOURCES += \
    $$PWD/keyboard/widgetKeyBoard.cpp \
    $$PWD/keyboard/QKeyPushButton.cpp
    # $$PWD/examplemyfocus.cpp #\

HEADERS  += \
    $$PWD/keyboard/widgetKeyBoard.h \
    $$PWD/keyboard/QKeyPushButton.h #\

TRANSLATIONS += \
    $$PWD/translations/virtualBoard_en.ts \
    $$PWD/translations/virtualBoard_fr.ts \
    $$PWD/translations/virtualBoard_it.ts \
    $$PWD/translations/virtualBoard_ru.ts

#TRANSLATIONS += \
#    src/andon_client/widgetkeyboard/translations/virtualBoard_en.ts \
#    src/andon_client/widgetkeyboard/translations/virtualBoard_fr.ts \
#    src/andon_client/widgetkeyboard/translations/virtualBoard_it.ts \
#    src/andon_client/widgetkeyboard/translations/virtualBoard_ru.ts



unix {    
    QMAKE_POST_LINK = lrelease widgetKeyboard2010.pro
}

RESOURCES += $$PWD/virtualboard.qrc \
    $$PWD/translations.qrc \
    $$PWD/sounds.qrc

#target.path = $$D:/Qt_projects/widgetKeyboard2014
#INSTALLS += target

#CODECFORTR = UTF-8
