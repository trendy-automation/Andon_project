#include "websocket_class.h"
#include "qwebchannel.h"
#include <QtWebSockets/QWebSocketServer>
#include "websocketclientwrapper.h"
#include "websockettransport.h"
#include "dbwrapper.h"
#include "message_handler.h"
#include "single_apprun.h"
#include "ketcp_object.h"
#include "nodes_manager.h"


//#include <functional>

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QSqlQuery>

#include <QDataStream>
#include <QVector>
#include <QBitArray>




/*
class SubClient : public SubscriptionHandler
{
    void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
    {
        Q_UNUSED(handle);
        Q_UNUSED(val);
        Q_UNUSED(attr);
        std::cout << "Received DataChange event for Node " << node << std::endl;
        m_server->TriggerEvent(*m_event);
    }

public:
    UaServer *m_server;
    OpcUa::Event *m_event;
};
*/
template<class T>
void listenPort(T * obj, int port, int interval, int delay) {
    QTimer *listenPortTimer = new QTimer;
    QObject::connect(listenPortTimer,&QTimer::timeout,[obj,port,listenPortTimer,interval](){
        if (obj->listen(QHostAddress::AnyIPv4, port)) {
            qDebug()<<QString("%1: %2 port opened").arg(obj->objectName()).arg(port);
            listenPortTimer->stop();
            listenPortTimer->deleteLater();
        } else {
            qDebug()<<QString("%1: Failed to open port %2").arg(obj->objectName()).arg(port);
            listenPortTimer->start(interval);
        }
    });
    listenPortTimer->start(delay);
}

int main(int argc, char** argv)
{

    /*


    Node largeTestFolder = objects.AddFolder("ns=1;s=Large.Folder", "Large_Folder");
    for (int x = 0; x <= 1000; x++)
        largeTestFolder.AddObject(idx, "");

    Node testFolder = objects.AddFolder("ns=3;s=TestFolder", "TestFolder");
    testFolder.AddVariable("ns=3;s=TestNode.ReadWrite", "ReadWriteTest", Variant(0.1));

    //addView(server->GetNode(ObjectId::ViewsFolder), "ns=3;i=5000", "ClassTestObject");

    //Create event
    Node triggerVar = testFolder.AddVariable("ns=3;s=TriggerVariable", "TriggerVariable", Variant(0));
    Node triggerNode = testFolder.AddObject("ns=3;s=TriggerNode", "TriggerNode");
    server->EnableEventNotification();
    Event ev(ObjectId::BaseEventType); //you should create your own type
    ev.Severity = 2;
    ev.SourceNode = triggerNode.GetId();
    ev.SourceName = "Test event";
    ev.Time = DateTime::Current();
    ev.Message = LocalizedText("test event");
    server->TriggerEvent(ev);

    // Workaround for not having server side methods
    SubClient clt;
    clt.m_event = &ev;
    clt.m_server = &server;
    std::unique_ptr<Subscription> sub = server->CreateSubscription(100, clt);
    sub->SubscribeDataChange(triggerVar);

    // Test variables containing arrays of various types
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Boolean", "BoolArrayTest", std::vector<bool>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Byte", "ByteArrayTest", std::vector<uint8_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.SByte", "SByteArrayTest", std::vector<int8_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.DateTime", "DateTimeArrayTest", std::vector<DateTime>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Double", "DoubleArrayTest", std::vector<double>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Float", "FloatArrayTest", std::vector<float>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Int16", "Int16ArrayTest", std::vector<int16_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Int32", "Int32ArrayTest", std::vector<int32_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Int64", "Int64ArrayTest", std::vector<int64_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.String", "StringArrayTest", std::vector<std::string>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.UInt16", "UInt16ArrayTest", std::vector<uint16_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.UInt32", "UInt32ArrayTest", std::vector<uint32_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.UInt64", "UInt64ArrayTest", std::vector<uint64_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.NodeId", "NodeIdArrayTest", std::vector<NodeId>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.LocalizedText", "LocalizedTextArrayTest", std::vector<LocalizedText>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.ByteString", "ByteStringArrayTest", std::vector<ByteString>());
    //  testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.XmlElement", "XmlElementArrayTest", std::vector<XmlElement>());

    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Boolean", "BoolScalarTest", true);
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Byte", "ByteScalarTest", uint8_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.SByte", "SByteScalarTest", int8_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Int16", "Int16ScalarTest", int16_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Int32", "Int32ScalarTest", int32_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Int64", "Int64ScalarTest", int64_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.UInt16", "UInt16ScalarTest", uint16_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.UInt32", "UInt32ScalarTest", uint32_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.UInt64", "UInt64ScalarTest", uint64_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Double", "DoubleScalarTest", 0.0);
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Float", "FloatScalarTest", 0.0f);
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.String", "StringScalarTest", std::string());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.DateTime", "DateTimeScalarTest", DateTime());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.LocalizedText", "LocalizedTextScalarTest", LocalizedText());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.NodeId", "NodeIdScalarTest", NodeId());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.ByteString", "ByteStringScalarTest", ByteString());

    Node demoFolder = objects.AddFolder("ns=3;s=DemoFolder", "DemoFolder");
    Node timeTickerNode = demoFolder.AddVariable("ns=2;s=DemoFolder.TimeTicker", "TimeTicker", DateTime());

    QTimer tickTimer;
    tickTimer.setInterval(1000);
    QObject::connect(&tickTimer, &QTimer::timeout, [&timeTickerNode](){
        timeTickerNode.SetValue(DateTime::Current());
    });
    tickTimer.start();

    QObject::connect(qApp, &QApplication::aboutToQuit, [&server](){
        qDebug("Stopping server");
        server->Stop();
    });

    // Initialize ACControl
    ACControl control;
    control.initNodes(server);
*/

    QApplication a(argc, argv);
    SingleAppRun singleApp(&a);
    MessageHandler msgHndlr(a.applicationDirPath().contains("build")?"cp866":"cp1251");

    //TODO language in Qt Creator console
    //TODO to another Thread
//    MessageHandler msgHndlr("cp1251");
//    QThread *msgHandlerThread;
//    msgHndlr.moveToThread(msgHandlerThread);
//    msgHandlerThread->start();
/*
    for(int i = 1;i<16;++i) {
        QBitArray bitar(i,true);

        QByteArray bytesArray;
        QDataStream streem(&bytesArray,QIODevice::ReadWrite);
        qDebug()<< "bitar(" <<i <<")" <<bitar;
        streem<<bitar;
        bytesArray.remove(0,4);
        //streem.device()->reset();
        QDataStream streem2(&bytesArray,QIODevice::ReadWrite);

        QVector<bool> vec(i);
//        QBitArray bitar(i,true);
        streem2.readRawData((char*)vec.data(),(i+7)/8);
//        streem2.readBytes((char*)vec.data(),(i+7)/8);
        qDebug()<< "bytesArray(" <<i <<")" <<bytesArray;
//        streem2>>vec;

        qDebug()<< "vec(" <<i <<")"
        <<vec.fromStdVector(std::vector<bool>(bytesArray.begin(),bytesArray.end()));

        for(int b=0; b<bitar.count(); ++b)
            ;

        std::vector<bool> sv(reinterpret_cast<const char*>(bitar.data_ptr()),
                             reinterpret_cast<const char*>((int)(bitar.data_ptr())+i));
        qDebug()<< "vec(" <<i <<")"<<QVector<bool>::fromStdVector(sv);
    }
*/
    DBWrapper *andondb = new DBWrapper;
    if(!andondb->ConnectDB(QApplication::applicationDirPath(), DATABASE_FILE)) {
        qDebug() << "Cannot connect to DB";
        a.quit();
        return 0;
    }

//    OpcUa::UaServer *server= new OpcUa::UaServer(false /*debug*/);
//    server->SetServerName("Andon OPCUA server");
//    server->SetEndpoint("opc.tcp://localhost:43344");
//    server->SetServerURI("urn://exampleserver->qt-project.org");
//    server->Start();


    // Initialize NodesManager
//    qDebug() << "ConnectDB";
    NodesManager *opcNodes = new NodesManager;
//    qDebug() << "NodesManager";
    opcNodes->initNodes();
//    qDebug("opcNodes->initNodes(server);");
//    qDebug() << "initNodes";
    andondb->executeQuery("SELECT * FROM TBL_TCPDEVICES "
                          "WHERE DEVICE_TYPE='KBX100' AND ENABLED=1",[opcNodes](QSqlQuery* query){
            while(query->next()) {
                KeTcpObject * keObject = new KeTcpObject;
                for(int i=0;i<query->record().count();++i)
                    keObject->setProperty(query->record().fieldName(i).toLatin1(),
                                          query->record().field(i).value());
                opcNodes->loadKeObject(keObject);
                keObject->startConnecting();
            }
        });




//    QObject::connect(qApp, &QApplication::aboutToQuit, [server](){
//        qDebug("Stopping server");
//        server->Stop();
//    });




    /*****************************************
     * Start WEBSOCKET
     *****************************************/

    {
        qDebug()<<"lambda WEBSOCKET start";
//        QJsonDocument jdocWebsocketPort(QJsonDocument::fromJson(resp.toString().toUtf8()));
//        QJsonArray tableArray = jdocWebsocketPort.array();
//        QJsonObject recordObject=tableArray.at(0).toObject();
//        int websocketPort = recordObject["WEBSOCKET_PORT"].toInt();
        int websocketPort = 12344;
        QWebSocketServer * server = new QWebSocketServer(QStringLiteral("QWebChannel server of opcua"),
                                                         QWebSocketServer::NonSecureMode);
        server->setObjectName("WebSocketServer");
        listenPort<QWebSocketServer>(server,websocketPort,3000,2000);
        WebSocketClientWrapper clientWrapper(server);
        WebSocketClass WScls;
        QWebChannel channel;
        channel.registerObject(QStringLiteral("clientWeb"), &WScls);
        QObject::connect(&clientWrapper, &WebSocketClientWrapper::clientConnected,
                         &channel, &QWebChannel::connectTo);

        qDebug()<<"lambda WEBSOCKET fineshed";

    }

    return a.exec();

}

