QT  = core core-private sql-private

QTDIR_build {
# This is only for the Qt build. Do not use externally. We mean it.
PLUGIN_TYPE = sqldrivers
#PLUGIN_CLASS_NAME = QCounterGesturePlugin
#PLUGIN_EXTENDS = -
load(qt_plugin)
CONFIG += install_ok
} else {

#TEMPLATE = lib
CONFIG += plugin

target.path += $$[QT_INSTALL_PLUGINS]/sqldrivers
INSTALLS += target

}
DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
