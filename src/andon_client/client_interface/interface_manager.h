#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>
//#include <QtScript>
#include <QJSEngine>
#include "../widgetkeyboard/keyboard/widgetKeyBoard.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QBitArray>

#include "client_rpcutility.h"
typedef QList<int> intList;
struct Property
{
    QByteArray name;
    QVariant value;
};
struct Script
{
    QByteArray name;
    QString text;
};
typedef QHash<intList, int> intList2IntHash;
inline uint qHash( const intList myList )
{
    if (myList.isEmpty())
       return 0;
    uint result = qHash(QString().number(myList.first()));
    for (int i=1;i<myList.count();++i)
        result ^= qHash(QString().number(myList.at(i)));
    return result;
}

/*
//inline uint qHash( const QVariantList myList )
//{
//    if (myList.isEmpty())
//       return 0;
//    uint result;
//    switch (myList.first().type())
//    {
//    case QVariant::Int:
//        result = qHash(QString().number(myList.first().toInt()));
//        for (int i=1;i<myList.count();++i)
//            result ^= qHash(QString().number(myList.at(i).toInt()));
//        break;
//    case QVariant::Double:
//        result = qHash(QString().number(myList.first().toInt()));
//        for (int i=1;i<myList.count();++i)
//            result ^= qHash(QString().number(myList.at(i).toInt()));
//        break;
//    case QVariant::String:
//        result = qHash(myList.first().toString());
//        for (int i=1;i<myList.count();++i)
//            result ^= qHash(myList.at(i).toString());
//        break;
//    case QVariant::Invalid:
//        qDebug()<<"Invalid Property in qHash"<<myList.first();
//        break;
//    default:
//        qDebug()<<"unknown type in qHash"<<myList.first();
//        break;
//    }
//    return result;
//}
*/

class InterfaceManager : public QMainWindow
{
    Q_OBJECT
public:
    explicit InterfaceManager(QWidget *parent = 0);
    ~InterfaceManager();

signals:
    void InputFinished(const QString &InputedText);
    void interfaceLoaded();
    void interfaceCreated();

public slots:
    void setEngine(QJSEngine *SharedEngine);
    void setServerRpc(ClientRpcUtility *SharedServerRpc);
    void loadInterface(const QVariant &resp);
    void loadProperties(const QVariant &resp);
    void loadScripts(const QVariant &resp);
    void loadWidgets(const QVariant &resp);
    void proccessPressed(QAbstractButton *clickedBtn);
    void refreshScreen(int ScreenNumber, bool showFlg=true);
    void refreshBtn(QAbstractButton* btn);
    void resetScreen();
    QObject *addButton(const QVariant &resp, const int &ScreenNum=0, bool RefreshScr=true);
    QObject *copyButton(const QVariant &resp, const QString &ObjectName="DEFAULT_CURRENT_STATUS");
    void updateButtons(const QVariant &resp);
    //Virtual Keyboard
    bool eventFilter(QObject *obj, QEvent *e);
    void InitVKeyboard();
    void KeyboardShow(QString LabelText="Input comment", QString InputText="", QString Timeout="0");
    void deleteButton(const QString &objectName);


private:
    void hide_keyboard();
    void evalScriptList(const QList<int> &scriptList);
    void evalScriptList(const QStringList &scriptList);
    void setupWidgets();
    void StepFinished(int StepNumber);
    //QJSEngine *JSEngine;
    QJSEngine* engine;
    ClientRpcUtility *RpcTcp;
    QBitArray steps;
    QList<int> currentChain;
    intList2IntHash StackedChains;
    QStackedWidget *Screens = new QStackedWidget;
    QButtonGroup *buttonGroup = new QButtonGroup;
    QMap <int, Property> Properties;
    QMap <int, Script> Scripts;
    QList<int> startScripts;
    QList<int> lastBtnScripts;
    //QObject * currentStatus;
    //Virtual Keyboard
    widgetKeyBoard  *myKeyboard;
    QWidget *widgetkeyboard;
    QLabel *vKeyboardLabel;
    QLineEdit  *InputLine;
    QVBoxLayout *keylay;
    int VKeyScreen=-1;
    QTimer * updateTimer = new QTimer;

};

#endif // INTERFACE_MANAGER_H
