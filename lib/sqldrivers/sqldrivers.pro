#src_plugins.subdir = $$PWD/plugins
#src_plugins.target = sub-plugins
#src_plugins.depends = src_sql src_xml src_network

#QT_FOR_CONFIG += network
#QT_FOR_CONFIG += sql

#qtConfig(sql-ibase)    : SUBDIRS += ibase

TEMPLATE = subdirs
SUBDIRS += ibase
#contains(sql-plugins, ibase)	: SUBDIRS += ibase
