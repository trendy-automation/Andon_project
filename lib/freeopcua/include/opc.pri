#include(C:/Qt/boost_1_60_0/boost.pri)
include(C:/Qt/boost_1_61_0/boost.pri)
INCLUDEPATH += $$PWD
win32 {
#    INCLUDEPATH += C:/Boost/include
#    LIBS += -LC:/Boost/lib \
#            -lboost_serialization-mgw49-mt-d-1_61 \
#            -lboost_filesystem-mgw49-mt-d-1_61 \
#            -lboost_system-mgw49-mt-d-1_61 \
#            -lws2_32  # winsock
}

HEADERS += \
    $$PWD/opc/common/addons_core/addon.h \
    $$PWD/opc/common/addons_core/addon_manager.h \
    $$PWD/opc/common/addons_core/addon_parameters.h \
    $$PWD/opc/common/addons_core/config_file.h \
    $$PWD/opc/common/addons_core/dynamic_addon.h \
    $$PWD/opc/common/addons_core/dynamic_addon_factory.h \
    $$PWD/opc/common/addons_core/errors.h \
    $$PWD/opc/common/class_pointers.h \
    $$PWD/opc/common/errors.h \
    $$PWD/opc/common/exception.h \
    $$PWD/opc/common/interface.h \
    $$PWD/opc/common/modules.h \
    $$PWD/opc/common/thread.h \
    $$PWD/opc/common/uri_facade.h \
    $$PWD/opc/ua/client/addon.h \
    $$PWD/opc/ua/client/binary_client.h \
    $$PWD/opc/ua/client/client.h \
    $$PWD/opc/ua/client/remote_connection.h \
    $$PWD/opc/ua/protocol/binary/common.h \
    $$PWD/opc/ua/protocol/binary/stream.h \
    $$PWD/opc/ua/protocol/attribute_ids.h \
    $$PWD/opc/ua/protocol/channel.h \
    $$PWD/opc/ua/protocol/data_value.h \
    $$PWD/opc/ua/protocol/datetime.h \
    $$PWD/opc/ua/protocol/describe.h \
    $$PWD/opc/ua/protocol/endpoints.h \
    $$PWD/opc/ua/protocol/enums.h \
    $$PWD/opc/ua/protocol/expanded_object_ids.h \
    $$PWD/opc/ua/protocol/extension_identifiers.h \
    $$PWD/opc/ua/protocol/guid.h \
    $$PWD/opc/ua/protocol/input_from_buffer.h \
    $$PWD/opc/ua/protocol/message_identifiers.h \
    $$PWD/opc/ua/protocol/monitored_items.h \
    $$PWD/opc/ua/protocol/node_management.h \
    $$PWD/opc/ua/protocol/nodeid.h \
    $$PWD/opc/ua/protocol/object_ids.h \
    $$PWD/opc/ua/protocol/protocol.h \
    $$PWD/opc/ua/protocol/protocol_auto.h \
    $$PWD/opc/ua/protocol/reference_ids.h \
    $$PWD/opc/ua/protocol/secure_channel.h \
    $$PWD/opc/ua/protocol/session.h \
    $$PWD/opc/ua/protocol/status_codes.h \
    $$PWD/opc/ua/protocol/string_utils.h \
    $$PWD/opc/ua/protocol/strings.h \
    $$PWD/opc/ua/protocol/types.h \
    $$PWD/opc/ua/protocol/types_manual.h \
    $$PWD/opc/ua/protocol/utils.h \
    $$PWD/opc/ua/protocol/variable_access_level.h \
    $$PWD/opc/ua/protocol/variant.h \
    $$PWD/opc/ua/protocol/variant_visitor.h \
    $$PWD/opc/ua/protocol/view.h \
    $$PWD/opc/ua/server/addons/address_space.h \
    $$PWD/opc/ua/server/addons/asio_addon.h \
    $$PWD/opc/ua/server/addons/common_addons.h \
    $$PWD/opc/ua/server/addons/endpoints_services.h \
    $$PWD/opc/ua/server/addons/opc_tcp_async.h \
    $$PWD/opc/ua/server/addons/opcua_protocol.h \
    $$PWD/opc/ua/server/addons/services_registry.h \
    $$PWD/opc/ua/server/addons/standard_address_space.h \
    $$PWD/opc/ua/server/addons/subscription_service.h \
    $$PWD/opc/ua/server/addons/xml_ns.h \
    $$PWD/opc/ua/server/address_space.h \
    $$PWD/opc/ua/server/endpoints_services.h \
    $$PWD/opc/ua/server/opc_tcp_async.h \
    $$PWD/opc/ua/server/server.h \
    $$PWD/opc/ua/server/services_registry.h \
    $$PWD/opc/ua/server/standard_address_space.h \
    $$PWD/opc/ua/server/subscription_service.h \
    $$PWD/opc/ua/services/attributes.h \
    $$PWD/opc/ua/services/endpoints.h \
    $$PWD/opc/ua/services/method.h \
    $$PWD/opc/ua/services/node_management.h \
    $$PWD/opc/ua/services/services.h \
    $$PWD/opc/ua/services/subscriptions.h \
    $$PWD/opc/ua/services/view.h \
    $$PWD/opc/ua/connection_listener.h \
    $$PWD/opc/ua/errors.h \
    $$PWD/opc/ua/event.h \
    $$PWD/opc/ua/global.h \
    $$PWD/opc/ua/model.h \
    $$PWD/opc/ua/node.h \
    $$PWD/opc/ua/server_operations.h \
    $$PWD/opc/ua/socket_channel.h \
    $$PWD/opc/ua/subscription.h



