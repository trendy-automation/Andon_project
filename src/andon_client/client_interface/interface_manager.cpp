#include "interface_manager.h"
#include <QGridLayout>
#include <QTime>
#include <cmath>

//#include <QQmlEngine>



InterfaceManager::InterfaceManager(QWidget *parent) : QMainWindow(parent)
{
    QObject::connect(buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>
                     (&QButtonGroup::buttonClicked), [=]
                     (QAbstractButton * clickedBtn){
            proccessPressed(clickedBtn);
        });
    this->setCentralWidget(Screens);

    updateTimer->setInterval(IM_BUTTONS_UPDATE_INTERVAL);
    //updateTimer->setSingleShot(false);

    steps.resize(4);
    steps.fill(false);
}

InterfaceManager::~InterfaceManager()
{
}

void InterfaceManager::StepFinished(int StepNumber)
{
    steps[StepNumber-1]=true;
    if (StepNumber==1) {
        emit interfaceLoaded();
        return;
    }
    bool res=true;
    for (int i=0;i<steps.count();++i)
        res &= steps[i];
    if (res) {
        setupWidgets();
        evalScriptList(startScripts);
        emit interfaceCreated();
    }
    //qDebug()<<"steps"<<steps;
}

void InterfaceManager::setServerRpc(ClientRpcUtility *SharedServerRpc)
{
    RpcTcp=SharedServerRpc;
}

void InterfaceManager::setEngine(QJSEngine* SharedEngine)
{
    engine=SharedEngine;
    engine->globalObject().setProperty("Screens",engine->newQObject(Screens));
    //engine->globalObject().setProperty("currentStatus",engine->newQObject(currentStatus));
    //engine->evaluate("var currentStatus={}");
}

void InterfaceManager::loadInterface(const QVariant &resp)
{
    //RPL, WL
    qDebug()<<"loadInterface start";
    //qDebug()<<"loadInterface resp"<<resp;
    QJsonDocument jdocInterface(QJsonDocument::fromJson(resp.toString().toUtf8()));
    QJsonArray tableArray = jdocInterface.array();
    QJsonObject recordObject;
    for (int i=0; i<tableArray.count();++i) {
        recordObject=tableArray.at(i).toObject();
        QGridLayout * gridlay = new QGridLayout;
        QStringList WidgetStrList = recordObject["WL"].toString().split(",");
        for (int j=0;j<WidgetStrList.count();++j) {
            QString wdgtName = QString("WIDGET_").append(WidgetStrList.at(j)).append("_").append(QString().number(i));
            QPushButton *pb;
            //qDebug()<<"new QPushButton " << wdgtName<<" on screen "<<i;
            pb=new QPushButton(Screens);
            pb->setObjectName(wdgtName);
            pb->setProperty("ID",WidgetStrList.at(j).toInt());
            //qDebug()<<"new setProperty ID " << WidgetStrList.at(j).toInt();
            pb->setFont(QFont("Arial",16));
            pb->setMaximumSize(4000,4000);
            buttonGroup->addButton(pb);
            gridlay->addWidget(pb);
        }
        //addnewwidget
        QWidget * w= new QWidget;
        w->setLayout(gridlay);
        Screens->insertWidget(i,w);
        QString Rpl=recordObject["RPL"].toString();
        if (!Rpl.isEmpty()) {
            QStringList RootPathList = Rpl.split(",");
            //qDebug()<<"RootPathList"<<RootPathList;
            //qDebug()<<"RootPathList.isEmpty()"<<RootPathList.isEmpty();
            //if (RootPathList.size()>0)
            for (int j=0;j<RootPathList.count();++j) {
                QStringList RootPath=RootPathList.at(j).split(";");
                //qDebug()<<"RootPath"<<RootPath;
                QList<int> pblist;
                bool allOk(true);
                bool ok;
                for (int x = 0; x < RootPath.count() && allOk; x++) {
                //    qDebug()<<"integer? RootPath.at(x)"<<RootPath.at(x);
                pblist.append(RootPath.at(x).toInt(&ok));
                allOk &= ok;
            }
            if (!allOk) {
                qDebug()<<"loadInterface str=>int conversion error";
            }
            //qDebug()<<"StackedChains.insert"<<pblist<<"Screen"<<i;
            StackedChains.insert(pblist,i);
            }
        }
    }
//    engine->globalObject().setProperty("currentStatus",engine->newQObject(new QObject()));
    if(engine)
        engine->evaluate("var currentStatus={}");

    //for (int k=0;k<buttonGroup->buttons().count();++k)
    //    qDebug()<<k<<buttonGroup->buttons().at(k)->objectName();


    StepFinished(1);
    qDebug()<<"loadInterface fineshed";
}

void InterfaceManager::loadProperties(const QVariant &resp)
{
    //IDP, PA, PV
    qDebug()<<"loadProperties start";
    //qDebug()<<"loadProperties resp "<<resp;
    QJsonDocument jdocInterface(QJsonDocument::fromJson(resp.toString().toUtf8()));
    QJsonObject recordObject;
    QJsonArray tableArray  = jdocInterface.array();
    for (int i=0; i<tableArray.count();++i) {
        recordObject=tableArray.at(i).toObject();
        Property currentProperty={recordObject["PA"].toString().toLatin1(),QJsonValue(recordObject["PV"]).toVariant()};
        Properties.insert(recordObject["IDP"].toInt(),currentProperty);
    }
    StepFinished(2);
    qDebug()<<"loadProperties fineshed";
}

void InterfaceManager::loadScripts(const QVariant &resp)
{
    //IDS, ST
    qDebug()<<"loadScripts start";
    //qDebug()<<"loadScripts resp "<<resp;
    QJsonDocument jdocInterface(QJsonDocument::fromJson(resp.toString().toUtf8()));
    QJsonArray tableArray  = jdocInterface.array();
    for (int i=0; i<tableArray.count();++i) {
        QJsonObject recordObject=tableArray.at(i).toObject();
        Script currentScript = {recordObject["SN"].toString().toLatin1(),recordObject["ST"].toString()};
        Scripts.insert(recordObject["IDS"].toInt(),currentScript);
    }
    StepFinished(3);
    qDebug()<<"loadScripts fineshed";
}

void InterfaceManager::loadWidgets(const QVariant &resp)
{
    //IDW, OT, PL, SL
    qDebug()<<"loadWidgets start";
    //qDebug()<<"loadWidgets resp"<<resp;
    QJsonDocument jdocWidgets(QJsonDocument::fromJson(resp.toString().toUtf8()));
    //qDebug()<<"jdocWidgets" << jdocWidgets;
    QJsonArray tableArray = jdocWidgets.array();
    //qDebug()<<"tableArray "<<tableArray;
    for (int i=0; i<tableArray.count();++i) {
        QJsonObject recordObject=tableArray.at(i).toObject();
        int idw= recordObject["IDW"].toInt();
        QStringList scriptList = recordObject["SL"].toString().split(",");
        switch (idw) {
            case -1:
                lastBtnScripts.clear();
                for (int j=0;j<scriptList.count();++j)
                    lastBtnScripts.append(scriptList.at(j).toInt());
                break;
            case 0:
                for (int j=0;j<scriptList.count();++j)
                    startScripts.append(scriptList.at(j).toInt());
                break;
            default:
                QString wdgtName = QString("WIDGET_").append(QString().number(idw)).append("_\\d+");
                QList<QPushButton*>pblst= Screens->findChildren<QPushButton*>(QRegularExpression(wdgtName));
                if (pblst.isEmpty())
                    qDebug()<<"Widgets" << wdgtName << "not found";
                else
                    for (int k=0;k<pblst.count();++k) {
                        QString objectText=recordObject["OT"].toString();
                        pblst.at(k)->setText(objectText);
                        pblst.at(k)->setProperty("OBJECT_TEXT",objectText);
                        pblst.at(k)->setProperty("PROPERTY_LIST",recordObject["PL"].toString().split(","));
                        pblst.at(k)->setProperty("SCRIPT_LIST",scriptList);
                    }
                break;
        }
    }
    qDebug()<<"loadWidgets fineshed";
    StepFinished(4);
}

void InterfaceManager::setupWidgets()
{
    qDebug()<<"setupWidgets start";
    QList<QAbstractButton*> pblst=buttonGroup->buttons();
    if (pblst.isEmpty()) {
        qDebug()<<"Widgets not found";
        return;
    }
    for (int i=0; i<pblst.count();++i) {
        qDebug()<<"setupWidget " << pblst.at(i)->objectName()
                << pblst.at(i)->text() ;
        QStringList propertyList = pblst.at(i)->property("PROPERTY_LIST").toStringList();
        if (propertyList!=QStringList()){
            for (int j=0;j<propertyList.count();++j) {
                int propertyId=propertyList.at(j).toInt();
                if (propertyId!=0) {
                    pblst.at(i)->setProperty(Properties.value(propertyId).name,
                                             Properties.value(propertyId).value);
                    if (Properties.value(propertyId).name=="SCRIPTS2LIST") {
                        QStringList scriptList=pblst.at(i)->property("SCRIPT_LIST").toStringList();
                        if (Properties.value(propertyId).value=="lastBtnScripts")
                            for (int k=0;k<scriptList.count();++k)
                                lastBtnScripts.append(scriptList.at(k).toInt());
                        if (Properties.value(propertyId).value=="startScripts")
                            for (int k=0;k<scriptList.count();++k)
                                startScripts.append(scriptList.at(k).toInt());
                    }
                    if (Properties.value(propertyId).name=="OBJECT_NAME")
                        pblst.at(i)->setObjectName(Properties.value(propertyId).value.toByteArray());
                }
            }
        }
        pblst.at(i)->setProperty("PROPERTY_LIST",QVariant());
    }
    for (int i=0;i<Screens->count();++i)
        refreshScreen(i,false);
    Screens->addWidget(widgetkeyboard);
    qDebug()<<"setupWidgets fineshed";
}

void InterfaceManager::refreshBtn(QAbstractButton *btn)
{
    int const FontSizeK=100;
    int const FontSize0=15;


    //qDebug()<<"btn OBJECT_TEXT"<<btn->property("OBJECT_TEXT").toString();
    if (btn->property("OBJECT_TEXT").toString()!= QString("")) {
        if (btn->property("OBJECT_TEXT")!= btn->property("LAST_OBJECT_TEXT")) {
            QString btnText = btn->property("OBJECT_TEXT").toString();
            btn->setProperty("LAST_OBJECT_TEXT",btnText);
            btnText.replace("\\n","\n");
            if (btnText.length()>12) {
                QRegExp rx("\\s+\\S{4}");
                int pos=0;
                while ((pos = rx.indexIn(btnText, pos)) != -1) {
                        btnText.replace(pos,1,"\n");
                        pos += rx.matchedLength();
                    }
                    btnText.replace("\n\n","\n").replace("\n\n","\n").replace("_"," ");
                }
                btn->setText(btnText);
                QFont tmpfont(btn->font());
                tmpfont.setPointSize((FontSizeK/btn->text().size()+FontSize0));
                btn->setFont(tmpfont);
        }
    }
        QString StyleSheet=btn->property("STYLESHEET").toString();
        if (StyleSheet.isEmpty())
            StyleSheet="QPushButton:pressed{background-color:orange;}QPushButton{background-color:white;}";
            btn->setStyleSheet(StyleSheet);

        QString VisibleScript=btn->property("VISIBLE_SCRIPT").toString();
        if (!VisibleScript.isEmpty()) {
            if (!(engine->evaluate(VisibleScript).toBool() &&
                      btn->property("OBJECT_TEXT").isValid()))
                btn->hide();
            else
                btn->show();
//            qDebug()<<btn->text()<<"VISIBLE_SCRIPT="<<btn->property("VISIBLE_SCRIPT").toString()<<
//                      "="<<(engine->evaluate(VisibleScript).toBool() &&
//                            btn->property("OBJECT_TEXT").isValid());
        }


}


void InterfaceManager::refreshScreen(int ScreenNumber, bool showFlg)
{
    qDebug()<<"RefreshScreen"<<ScreenNumber;
        if (!Screens->widget(ScreenNumber)){
            qDebug()<<"No screen number"<<ScreenNumber;
            return;
        }
        if (!Screens->widget(ScreenNumber)->layout()) {
            qDebug()<<"No layout on screen"<<ScreenNumber;
            return;
        }
            int btnCount=Screens->widget(ScreenNumber)->layout()->count();
            int hc = 0;
            QGridLayout * gridlay = new QGridLayout;
            QPushButton *btn;
            for (int i=0;i<btnCount;++i) {
                btn = (QPushButton *)Screens->widget(ScreenNumber)->layout()->itemAt(i)->widget();
                refreshBtn(btn);
                if (btn->isHidden())
                    hc++;
            }
            int n = ceil(sqrt(btnCount-hc));
            qDebug() << "btnCount =" << btnCount << ";hc =" << hc << ";n =" << n;
            int pos;
            hc=0;
            if (n!=0)
            for (int i=0;i<btnCount;++i) {
                btn = (QPushButton *)Screens->widget(ScreenNumber)->layout()->takeAt(0)->widget();
                if (btn->isHidden()){
                    pos=n*n;
                    hc++;
                }
                else
                    pos=i-hc;
                //qDebug() << "pos =" << pos;
                //qDebug() << "pos/n =" << pos/n;
                //qDebug() << "pos%n =" << pos%n;
                gridlay->addWidget(btn, pos/n, pos%n, 1, 1);
            }


            delete Screens->widget(ScreenNumber)->layout();
            Screens->widget(ScreenNumber)->setLayout(gridlay);
            //qDebug() << "if (showFlg)";
            if (showFlg)
                Screens->setCurrentIndex(ScreenNumber);
                //Screens->currentWidget()->showFullScreen();
}

void InterfaceManager::proccessPressed(QAbstractButton *clickedBtn)
{
    //engine->globalObject().setProperty("USER_COMMENT", QJSValue());
    if (clickedBtn->property("SKIP_PP").toInt()==1) {
        evalScriptList(clickedBtn->property("SCRIPT_LIST").toStringList());
        return;
    }
    //qDebug()<<"ProccessPressed"<<clickedBtn->property("OBJECT_TEXT").toString();
    QList<QByteArray> dynamicProperties = clickedBtn->dynamicPropertyNames();
    dynamicProperties.removeOne("ID");
    dynamicProperties.removeOne("OBJECT_TEXT");
    dynamicProperties.removeOne("LAST_OBJECT_TEXT");
    dynamicProperties.removeOne("VISIBLE_SCRIPT");
    dynamicProperties.removeOne("STYLESHEET");
    dynamicProperties.removeOne("SCRIPT_LIST");
    dynamicProperties.removeOne("SKIP_LB_SCRIPT");
    dynamicProperties.removeOne("SKIP_ON");
    dynamicProperties.removeOne("_q_styleSheetWidgetFont");
    dynamicProperties.removeOne("_q_stylestate");
    dynamicProperties.removeOne("_q_stylerect");
    dynamicProperties.removeOne("_q_isdefault");
    //currentStatus=engine->globalObject().property("currentStatus").toQObject();
    for (int i=0;i<dynamicProperties.count();++i)
        engine->evaluate(QString("currentStatus.").append(dynamicProperties.at(i)).append("='").
                         append(clickedBtn->property(dynamicProperties.at(i)).toString()).append("'"));
//        currentStatus->setProperty(dynamicProperties.at(i),clickedBtn->property(dynamicProperties.at(i)));
    if (Screens->widget(StackedChains.value(currentChain))->layout()->count()>1 &&
            clickedBtn->property("SKIP_ON").toInt()!=1) {
//        currentStatus->setProperty("OBJECT_TEXT",
//                                    currentStatus->property("OBJECT_TEXT").toString().
        QString scriptCurStatus;
        if (engine->evaluate("typeof currentStatus.OBJECT_TEXT=='undefined'").toBool())
            scriptCurStatus = "currentStatus.OBJECT_TEXT='";
        else
            scriptCurStatus = "currentStatus.OBJECT_TEXT=currentStatus.OBJECT_TEXT+' '+'";
        engine->evaluate(scriptCurStatus.append(clickedBtn->property("OBJECT_TEXT").toString()).append("'"));
    }
//    currentStatus->property("OBJECT_TEXT").toStringList()
//    .append(clickedBtn->property("OBJECT_TEXT").toString()));
//    engine->globalObject().setProperty("currentStatus",engine->newQObject(currentStatus));
    engine->globalObject().setProperty("btn",engine->newQObject(clickedBtn));
    engine->globalObject().setProperty("currentScreen",engine->newQObject(Screens->currentWidget()->layout()));
    if (!clickedBtn->property("SCRIPT_LIST").toStringList().at(0).isEmpty())
    //    if (clickedBtn->property("SCRIPT_LIST").toStringList()!=QStringList())
        evalScriptList(clickedBtn->property("SCRIPT_LIST").toStringList());
    engine->globalObject().setProperty("btn",QJSValue());
    engine->globalObject().setProperty("currentScreen",QJSValue());

    //QList<int> currentChain = StackedChains.key(Screens->currentIndex());

    if (clickedBtn->property("ID")!=QVariant()) {
        if (clickedBtn->property("ID").toInt()!=0)
            currentChain.append(clickedBtn->property("ID").toInt());}
    else
        qDebug() << "No id property at "<<StackedChains;
    if (StackedChains.contains(currentChain)) {
        switch (Screens->widget(StackedChains.value(currentChain))->layout()->count()) {
        case 0:
            qDebug() << "Empty Screen="<<StackedChains.value(currentChain)<<"on currentChain="<<currentChain;
            resetScreen();
            break;
        case 1:
            qDebug() << "Single widget pressed simulated"<<StackedChains.value(currentChain);
            //Screens->setCurrentIndex(StackedChains.value(currentChain));
            proccessPressed((QAbstractButton*)Screens->widget(StackedChains.value(currentChain))->layout()->itemAt(0)->widget());
            break;
        default:
            refreshScreen(StackedChains.value(currentChain));
            //Screens->setCurrentIndex(StackedChains.value(currentChain));
            break;
        }
    }
    else {
        //qDebug() << "evaluate lastBtnScripts; currentChain="<<currentChain;
        engine->evaluate(QString("currentStatus.EVENT_ID='")
                         .append(QTime().currentTime().toString("hh:mm:ss.zzz")).append("'"));
        engine->evaluate(QString("currentStatus.TIME='")
                         .append(QTime().currentTime().toString("hh:mm")).append("'"));
        engine->evaluate("currentStatus.STATION_IP=':CLIENT_IP'");
        //currentStatus->setProperty("EVENT_ID",QTime().currentTime().toString("hh:mm:ss.zzz"));
        //currentStatus->setProperty("TIME",QTime().currentTime().toString("hh:mm"));
        //currentStatus->setProperty("STATION_IP",":CLIENT_IP");
        //qDebug() << "currentStatus->dynamicPropertyNames()" << currentStatus->dynamicPropertyNames();
        if (clickedBtn->property("SKIP_LB_SCRIPT").toInt()!=1)
            if (!lastBtnScripts.isEmpty())
                evalScriptList(lastBtnScripts);
        //RpcTcp->ServerExecute("StartSms", QVariantList()<<engine->evaluate("JSON.stringify(currentStatus)").toString());
        resetScreen();
    }
}

void InterfaceManager::resetScreen()
{
    currentChain.clear();
    //delete currentStatus;
    //currentStatus=new QObject;
    //engine->globalObject().setProperty("currentStatus",QJSValue());
    engine->evaluate("var currentStatus={}");
    Screens->setCurrentIndex(StackedChains.value(intList()));
}

void InterfaceManager::evalScriptList(const QList<int> &scriptList)
{
    for (int i = 0; i<scriptList.count(); ++i) {
        qDebug() << "evaluate script"<<scriptList.at(i)<<Scripts.value(scriptList.at(i)).text;
        QJSValue result = engine->evaluate(Scripts.value(scriptList.at(i)).text);
         if (result.isError())
             qDebug()<< "Uncaught exception at line"
                     << result.property("lineNumber").toInt()
                     << ":" << result.toString();
        //qDebug() << "evaluate script done";
    }
}

void InterfaceManager::evalScriptList(const QStringList &scriptList)
{
    for (int i = 0; i<scriptList.count(); ++i) {
        if (scriptList.at(i).toInt()>0){
            qDebug() << "evaluate script from QStringList"<<scriptList.at(i)<<Scripts.value(scriptList.at(i).toInt()).text;
            QJSValue result = engine->evaluate(Scripts.value(scriptList.at(i).toInt()).text);
             if (result.isError())
                 qDebug()<< "Uncaught exception at line"
                         << result.property("lineNumber").toInt()
                         << ":" << result.toString();
            //qDebug() << "evaluate script done";
        }
    }
}

QObject *InterfaceManager::addButton(const QVariant &resp, const int &ScreenNum, bool RefreshScr)
{
    qDebug() << "addButton";//<<resp;
    QPushButton * new_wgt = new QPushButton(Screens);
    new_wgt->setMaximumSize(4000,4000);

    QJsonDocument jdocProperties(QJsonDocument::fromJson(resp.toString().toUtf8()));
    //qDebug() << "jdocProperties"<<jdocProperties;
    QJsonObject jobjProperties(jdocProperties.object());
    //qDebug() << "jobjProperties"<<jobjProperties;
    if (!jobjProperties.contains("VISIBLE_SCRIPT"))
        jobjProperties.insert("VISIBLE_SCRIPT",1);
//    QMapIterator<QByteArray, QVariant> i(PropertiesMap);
//    //if (!PropertiesMap.contains("VISIBLE_SCRIPT"))
//    //    PropertiesMap.insert("VISIBLE_SCRIPT",1);
   for(QJsonObject::const_iterator iter = jobjProperties.begin();iter!=jobjProperties.end();  ++iter){
        if (iter.key()=="OBJECT_NAME")
            new_wgt->setObjectName(iter.value().toString());
        if (((iter.key()=="OEM_OBJECT_TEXT") || (iter.key()=="OBJECT_TEXT")) && new_wgt->text().isEmpty())
            new_wgt->setText(iter.value().toString());
        new_wgt->setProperty(iter.key().toLatin1(),iter.value().toVariant());
    }
    buttonGroup->addButton(new_wgt);
    Screens->widget(ScreenNum)->layout()->addWidget(new_wgt);
    engine->evaluate("IM").setProperty(new_wgt->objectName(),engine->newQObject(new_wgt));
    if (RefreshScr)
        this->refreshScreen(ScreenNum);

    if (!updateTimer->isActive()){
        updateTimer->start();
        QObject::connect(updateTimer,&QTimer::timeout,[=](){
            RpcTcp->Query2Json("SELECT OBJECT_NAME, OBJECT_TEXT, USER_COMMENT "
                                                         " FROM CLIENT_ACTIVE_BUTTONS(:CLIENT_IP)",
                                  [=](QVariant resp){updateButtons(resp);});
        });
    }
    return new_wgt;
}

QObject *InterfaceManager::copyButton(const QVariant &resp, const QString &ObjectName)
{
    qDebug() << "copyButton" ;//<<resp.toString();
    QPushButton * original_wgt = Screens->findChild<QPushButton*>(ObjectName);
    if (original_wgt) {
        QList <QByteArray> DynProp=original_wgt->dynamicPropertyNames();
        //qDebug() << "DynProp" << DynProp;
        DynProp.removeOne("OBJECT_NAME");
        DynProp.removeOne("OBJECT_TEXT");
        //DynProp.removeOne("LAST_OBJECT_TEXT");
        DynProp.removeOne("VISIBLE_SCRIPT");
        DynProp.removeOne("_Q_STYLESHEETWIDGETFONT");
        // !DynProp.removeOne("SCRIPT_LIST");
        // !DynProp.removeOne("ID");
        //QMap<QByteArray,QVariant> PropertiesMap;
        //for (int i=0;i<DynProp.count();++i) {
        //    PropertiesMap[DynProp.at(i).toUpper()]=original_wgt->property(DynProp.at(i));
        QJsonDocument jdocProperties(QJsonDocument::fromJson(resp.toString().toUtf8()));
        //qDebug() << "jdocProperties"<<jdocProperties;
        if (jdocProperties.isObject()) {
            qDebug() << "jdocProperties is Object";
//            QJsonObject jobjProperties = jdocProperties.object();
//            QVariantMap VarMapProperties=jobjProperties.toVariantMap();
//            QMapIterator<QString, QVariant> i(VarMapProperties);
//            while (i.hasNext()) {
//                i.next();
//                PropertiesMap[i.key().toLatin1()]=i.value();
//            }
            QJsonObject jobjPropertiesTmp(jdocProperties.object());
            for (int i=0;i<DynProp.count();++i)
                jobjPropertiesTmp[DynProp.at(i).toUpper()]=QJsonValue::fromVariant(original_wgt->property(DynProp.at(i)));
            //qDebug() << "jobjPropertiesTmp"<<jobjPropertiesTmp;
            QJsonDocument jsonProp(jobjPropertiesTmp);
            return addButton(jsonProp.toJson(QJsonDocument::Compact),0,true);

            if (jdocProperties.isArray()) {
                 qDebug() << "copy fault jdocProperties is Array";
    //            QJsonArray tableArray  = jdocProperties.array();
    //            for (int i=0; i<tableArray.count();++i) {
    //                QJsonObject recordObject=tableArray.at(i).toObject();
    //                for (int j=0; i<recordObject.keys().count();++j) {
    //                    QString key = recordObject.keys().at(j);
    //                    PropertiesMap[key.toLatin1()]=recordObject.value(key).toVariant();
    //                }
    //            }
            }
        }
        qDebug() << "copy fault jdocProperties is not Object";
        return 0;
    } else
        {qDebug()<<"CopyWidget failed. no object" << ObjectName;
        return 0;
    }
}


void InterfaceManager::updateButtons(const QVariant &resp)
{
    qDebug()<<"updateTimer";// resp"<<resp;
    //OBJECT_NAME, OBJECT_TEXT, USER_COMMENT
    QJsonDocument jdocButtons(QJsonDocument::fromJson(resp.toString().toUtf8()));
    QJsonArray tableArray  = jdocButtons.array();
    if ((!resp.isValid()) || (resp.isNull()) || (tableArray.isEmpty())) {
        //qDebug()<<"!resp.isValid()"<<!resp.isValid()<<"resp.isNull()"<<resp.isNull()
        //        <<"tableArray.isEmpty()"<<tableArray.isEmpty();
        qDebug()<<"bad resp"<<resp;
        //updateTimer->stop();
        //updateTimer->disconnect();
        // //QObject::disconnect(updateTimer,0,0,0);
        //qDebug()<<"updateTimer stop";
        return;
    }

    for (int i=0; i<tableArray.count();++i) {
        QJsonObject recordObject=tableArray.at(i).toObject();
        if(recordObject.contains("OBJECT_NAME") && recordObject.contains("OBJECT_TEXT")
                && recordObject.contains("USER_COMMENT")) {
            QAbstractButton* updatedBtn=Screens->findChild<QAbstractButton*>(recordObject["OBJECT_NAME"].toString());
            if (updatedBtn){
                updatedBtn->setProperty("OBJECT_TEXT",recordObject["OBJECT_TEXT"].toString());
                updatedBtn->setProperty("USER_COMMENT",recordObject["USER_COMMENT"].toString());
                refreshBtn(updatedBtn);
            }
        }
    }
}

void InterfaceManager::deleteButton(const QString &objectName)
{
    QPushButton *btn = this->findChild<QPushButton*>(objectName);
    if (btn)
        btn->deleteLater();
}




void InterfaceManager::InitVKeyboard()
{
    myKeyboard = new widgetKeyBoard(false);
    myKeyboard->setWindowFlags(Qt::WindowStaysOnTopHint);
    myKeyboard->setZoomFacility(true);
    myKeyboard->enableSwitchingEcho(false); // enable possibility to change echo through keyboard
    myKeyboard->createKeyboard(); // only create keyboard
    widgetkeyboard = new QWidget(this);
    widgetkeyboard->setGeometry(this->geometry());
    keylay = new QVBoxLayout(widgetkeyboard) ;
    InputLine = new QLineEdit(myKeyboard);
    QFont tempfont=InputLine->font();
    tempfont.setPointSize(25);
    InputLine->setFont(tempfont);
    vKeyboardLabel = new QLabel;
    tempfont=vKeyboardLabel->font();
    tempfont.setPointSize(25);
    vKeyboardLabel->setFont(tempfont);
    keylay->addWidget(vKeyboardLabel,0);
    keylay->addWidget(InputLine,1);
    keylay->setAlignment(vKeyboardLabel,Qt::AlignTop);
    keylay->setAlignment(InputLine,Qt::AlignTop);
    widgetkeyboard->setLayout(keylay);
    myKeyboard->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnBottomHint);
    myKeyboard->setWindowTitle("Virtual Keyboard");
    //Qt::FramelessWindowHint |
//    Screens->insertWidget(-1,widgetkeyboard);
    QObject::connect( myKeyboard, &widgetKeyBoard::returnPressed, this , &InterfaceManager::hide_keyboard );
    qDebug() << "InitVKeyboard finished";
}

void InterfaceManager::KeyboardShow(QString LabelText,QString InputText,QString Timeout)
{
    widgetkeyboard->setProperty("LAST_SCREEN",Screens->currentIndex());
    Screens->setCurrentWidget(widgetkeyboard);
    vKeyboardLabel->setText(LabelText.replace("\\n","\n"));
    InputLine->setText(InputText);
    widgetkeyboard->installEventFilter(this);
    widgetkeyboard->repaint();
    myKeyboard->show(this);
    myKeyboard->move(0, this->height()- myKeyboard->height()-50); // to move keyboard
}

void InterfaceManager::hide_keyboard()
{
    /*
        function StoreComment (USER_COMMENT) {
        currentStatus.USER_COMMENT= USER_COMMENT;
        IM.InputFinished.disconnect(this,this.StoreComment);
        }
        IM.InputFinished.connect(this,StoreComment);
    */
    engine->evaluate(QString("currentStatus.USER_COMMENT='")
                     .append(InputLine->text().replace(","," ").replace("="," ").replace("'",""))
                     .append("'"));
    //currentStatus->setProperty("USER_COMMENT",InputLine->text().replace(","," ").replace("="," ").replace("'",""));
    myKeyboard->hide(true);
    Screens->setCurrentIndex(widgetkeyboard->property("LAST_SCREEN").toInt());
    emit InputFinished(InputLine->text().replace(","," ").replace("="," ").replace("'",""));
}

bool InterfaceManager::eventFilter(QObject *obj, QEvent *e) {
    if (e->type() == QEvent::WindowActivate)
        myKeyboard->activateWindow();
    return QObject::eventFilter(obj, e);
}

class AlarmProcessing
{
private:
    int Alarm(int Type);

};
