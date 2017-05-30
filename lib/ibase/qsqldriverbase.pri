QT  = core core-private sql-private

QTDIR_build {
# This is only for the Qt build. Do not use externally. We mean it.
PLUGIN_TYPE = sqldrivers
PLUGIN_CLASS_NAME = QIBaseSql
qsqlibase
load(qt_plugin)
CONFIG += install_ok
} else {
TEMPLATE = lib
CONFIG += plugin

#TARGET   = $$qtLibraryTarget($$TARGET)

target.path += $$[QT_INSTALL_PLUGINS]/sqldrivers
INSTALLS += target
#creator_target.name = Copying the target dll to Qt Creator plugins directory as well
#creator_target.input = $qtLbraryTarget(qsqlibase)
#creator_target.path  = $$(QTCREATOR_BIN_PATH)/plugins/sqldrivers/
#creator_target.CONFIG += no_check_exist
#creator_target.output = qsqlibase.dll
#creator_target.files = $$OUT_PWD/release/qsqlibase.dll
#QMAKE_EXTRA_COMPILERS += creator_target
#QMAKE_EXTRA_TARGETS += target creator_target
#INSTALLS += target creator_target
}
DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

