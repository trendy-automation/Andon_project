/*=============================================================================|
|  PROJECT SNAP7                                                         1.2.0 |
|==============================================================================|
|  Copyright (C) 2013, 2014 Davide Nardella                                    |
|  All rights reserved.                                                        |
|==============================================================================|
|  SNAP7 is free software: you can redistribute it and/or modify               |
|  it under the terms of the Lesser GNU General Public License as published by |
|  the Free Software Foundation, either version 3 of the License, or           |
|  (at your option) any later version.                                         |
|                                                                              |
|  It means that you can distribute your commercial software linked with       |
|  SNAP7 without the requirement to distribute the source code of your         |
|  application and without the requirement that your application be itself     |
|  distributed under LGPL.                                                     |
|                                                                              |
|  SNAP7 is distributed in the hope that it will be useful,                    |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
|  Lesser GNU General Public License for more details.                         |
|                                                                              |
|  You should have received a copy of the GNU General Public License and a     |
|  copy of Lesser GNU General Public License along with Snap7.                 |
|  If not, see  http://www.gnu.org/licenses/                                   |
|==============================================================================|
|                                                                              |
|  C++ Snap 7 classes Implementation                                           |
|                                                                              |
|=============================================================================*/

#include "snap7.h"
#include <QDebug>


//==============================================================================
// CLIENT
//==============================================================================

bool TS7Client::setIP(const QString &IPaddress)
{
    PLC_IP=IPaddress.toLatin1();
    //TODO: qregext
    return true;
}



void  TS7Client::run(void)
{
    qDebug()<<"TS7Client thread started";
    //MultivarsRead

    //TODO: ciclical reconnect
    ConnectTo(PLC_IP,0,2);
    //if ConnectTo==0 ... else ...

    //TODO: get interval from DB


//    QObject::connect(&MultiVarTimer,&QTimer::timeout,[=](){
//        CycMultiVars();
//    });

    //MultiVarTimer.start();
}

void TS7Client::CycMultiVars()
{
    MultiVarTimer.stop(); //???
    int res =ReadMultiVars(&ItemArray[0],ItemCount);
    //if (res!=0) qDebug()<<"Tags not updated. Error code:"<<res;
    MultiVarTimer.start();
}

void TS7Client::TagCycleStart()
{
    MultiVarTimer.start();
}

void TS7Client::TagCycleStop()
{
    MultiVarTimer.stop();
}


bool TS7Client::Item2S7addr(TS7DataItem &Item, QString &S7address, int BitNumber)
{
    const int S7WLBit     = 0x01;
    const int S7WLByte    = 0x02;
    const int S7WLWord    = 0x04;
    const int S7WLDWord   = 0x06;

    switch (Item.Area) {
    case S7AreaMK:
        S7address="M";
        break;
    case S7AreaDB:
        S7address="DB"+QString::number(Item.DBNumber)+".DB";
        break;
    default:
        return false;
    }

    switch (Item.WordLen) {
    case S7WLBit:
        if (BitNumber<0)
            return false;
        if (Item.Area==S7AreaDB)
            S7address.append("X");
        break;
    case S7WLByte:
        S7address.append("B");
        break;
    case S7WLWord:
        S7address.append("W");
        break;
    case S7WLDWord:
        S7address.append("D");
        break;
    default:
        return false;
    }
    S7address.append(QString::number(Item.Start));
    if (Item.WordLen==S7WLBit)
        S7address.append(".").append(QString::number(BitNumber));
    return true;

}

bool TS7Client::S7addr2Item(TS7DataItem &Item, const QString &S7address)
{
    //if (isS7address(S7address)){
    QRegExp isDBrx("DB[1-65535]\\.DB(X[0-65535]\\.[0-7]|B[0-65535]|W[0-65535]|D[0-65535])");
    QRegExp isMrx("M([1-65535]\\.[0-7]|B[0-65535]|W[0-65535]|D[0-65535])");
    if (isDBrx.exactMatch(S7address)) {

        //QRegExp caprx("DB([1-65535])\\.DB(X|B|W|D)([0-65535])\\.([0-7])");
        caprx.indexIn(S7address);
        Item.Area=S7AreaDB;
        Item.DBNumber = caprx.cap(0).toInt();
        Item.Start = caprx.cap(1).toInt();
        //        S7WLByte    = 0x02; S7WLWord    = 0x04; S7WLDWord   = 0x06;
        Item.WordLen = caprx.cap(2).replace("X","1").replace("B","2").replace("W","4").replace("D","6").toInt();
        Item.Amount = 1;
        Item.Result = -1;
        int size;
        if (Item.WordLen==S7WLBit) size = 1;
        if (Item.WordLen==S7WLByte) size = 1;
        if (Item.WordLen==S7WLWord) size = 2;
        if (Item.WordLen==S7WLDWord) size = 4;
        Item.pdata=new byte[size];
        //TODO check dblist!
        //key(caprx.cap(3).toInt(););
        return true;
    }
    return false;
}

//Tag("Alias").value
//Tag("S7address").value
//Tag( номерДБ(если тип по умолчанию), начальный байт, длина, номер бита=0(учитывается,если длина=0),тип=0x84(ДБ)).value

bool TS7Client::addTag(const QString &S7address, const QString &Alias)
{
    if ((S7address.isEmpty()) || (ItemCount>=MaxItemCount))
        return false;
        CustomKey NewKey(S7address,Alias);
        if (NewKey.isValid())
            if (!ItemMap.contains(NewKey))
                if (S7addr2Item(ItemArray[ItemCount+1],S7address)) {
                    ItemCount = ItemCount+1;
                    ItemMap.insert(NewKey,ItemCount);
                    return true;
                }
    return false;
}



bool TS7Client::addTag(int DBNumber,int Start,int size, QString &S7address, QString Alias,int Area,int Amount, int BitNumber)
{
    if (ItemCount>=MaxItemCount)
        return false;
        ItemArray[ItemCount+1].Area=Area;
        ItemArray[ItemCount+1].DBNumber=DBNumber;
        ItemArray[ItemCount+1].Start=Start;
        ItemArray[ItemCount+1].Amount=Amount;
        ItemArray[ItemCount+1].Result=-1;
        switch (size) {
        case 1: //0?
            if (BitNumber>-1)
                ItemArray[ItemCount+1].WordLen=S7WLBit;
            else
                ItemArray[ItemCount+1].WordLen=S7WLByte;
            break;
        case 2:
            ItemArray[ItemCount+1].WordLen=S7WLWord;
            break;
        case 4:
            ItemArray[ItemCount+1].WordLen=S7WLDWord;
            break;
        default:
            return false;
        }
        ItemArray[ItemCount+1].pdata=new byte[size];

        if (Item2S7addr(ItemArray[ItemCount+1],S7address,BitNumber)) {
            CustomKey NewKey(S7address,Alias);
            if (NewKey.isValid())
                if (!ItemMap.contains(NewKey)) {
                        ItemCount = ItemCount+1;
                        ItemMap.insert(NewKey,ItemCount);
                        return true;
                    }
        }
        return false;
}


//int ReadTag(QString TagKey,bool &ok)
bool TS7Client::ReadTag(const QString &TagKey, int &value)
{
    CustomKey NewKey(TagKey);
    if (ItemMap.contains(NewKey)) {
        int ItemNum = ItemMap.value(NewKey);
        NewKey=ItemMap.key(ItemNum);
        //CustomKey ActKey = CustomKey(ItemMap.key(ItemNum));
        int res=ItemArray[ItemNum].Result;
        //TODO: last CycMultiVars time !!!!
        if (res!=0) {
            if (res==-1)
                CycMultiVars();
            else
                qDebug()<<"Can not Read Tag"<<TagKey<<"Error code:"<<res;
        }
        return Item2Int(ItemArray[ItemNum],NewKey.S7address(),value);
    } else
        return false;
}

bool TS7Client::Item2Int (TS7DataItem Item, QString S7address, int &value)
{
    if ((Item.Amount!=1) || (Item.Result!=0))
        return false;
    int size;
    int res=0;
    switch (Item.WordLen) {
    case S7WLBit:
        size=1;
        break;
    case S7WLByte:
        size=1;
        break;
    case S7WLWord:
        size=2;
        break;
    case S7WLDWord:
        size=4;
        break;
    default:
        return false;
    }
    byte*Buf=static_cast<byte*>(Item.pdata);

    for (int i=0;i<size;++i)
        res = res | ((Buf[i]) << 8*(size - i-1));
    if (Item.WordLen==S7WLBit) {
        caprx.indexIn(S7address);
        value = res>>caprx.cap(3).toInt() & 1;
    } else
        value = res;


    return true;
}




void S7API extCliCompletion(void *usrPtr, int opCode, int opResult)
{
    TS7Client * Client = (TS7Client *) usrPtr;
    Client->CliCompletion(opCode, opResult);
}

void S7API TS7Client::CliCompletion(int opCode, int opResult)
{
    //TODO if opcode == DBRead
    if (opCode==1) {
        emit DBReadDone(DBTask);
        return;
    }
    qDebug() << "CliCompletion; opcode="<<opCode<<"opResult="<<opResult;
    //qDebug() << "CliCompletion Error!!";
}
//---------------------------------------------------------------------------
TS7Client::TS7Client()
{
    Client=Cli_Create();
    SetAsCallback(extCliCompletion, this);
    caprx=QRegExp("DB([1-65535])\\.DB(X|B|W|D)([0-65535])\\.([0-7])");

    MultiVarTimer.setInterval(1000);
    MultiVarTimer.setSingleShot(false);
    QObject::disconnect(&MultiVarTimer,&QTimer::timeout,this,&TS7Client::CycMultiVars);
    QObject::connect(&MultiVarTimer,&QTimer::timeout,this,&TS7Client::CycMultiVars);

    QObject::connect(this, &QThread::started, &MultiVarTimer,
                     static_cast<void (QTimer::*)()>(&QTimer::start));

}
//---------------------------------------------------------------------------
TS7Client::~TS7Client()
{
    Cli_Destroy(&Client);
}
//---------------------------------------------------------------------------
int TS7Client::Connect()
{
    return Cli_Connect(Client);
}
//---------------------------------------------------------------------------
int TS7Client::ConnectTo(const char *RemAddress, int Rack, int Slot)
{
    return Cli_ConnectTo(Client, RemAddress, Rack, Slot);
}
//---------------------------------------------------------------------------
int TS7Client::SetConnectionParams(const char *RemAddress, word LocalTSAP, word RemoteTSAP)
{
    return Cli_SetConnectionParams(Client, RemAddress, LocalTSAP, RemoteTSAP);
}
//---------------------------------------------------------------------------
int TS7Client::SetConnectionType(word ConnectionType)
{
    return Cli_SetConnectionType(Client, ConnectionType);
}
//---------------------------------------------------------------------------
int TS7Client::Disconnect()
{
    return Cli_Disconnect(Client);
}
//---------------------------------------------------------------------------
int TS7Client::GetParam(int ParamNumber, void *pValue)
{
    return Cli_GetParam(Client, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Client::SetParam(int ParamNumber, void *pValue)
{
    return Cli_SetParam(Client, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Client::ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_ReadArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::WriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ReadMultiVars(PS7DataItem Item, int ItemsCount)
{
    return Cli_ReadMultiVars(Client, Item, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::WriteMultiVars(PS7DataItem Item, int ItemsCount)
{
    return Cli_WriteMultiVars(Client, Item, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::DBRead(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_DBRead(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::DBWrite(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_DBWrite(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::MBRead(int Start, int Size, void *pUsrData)
{
    return Cli_MBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::MBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_MBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::EBRead(int Start, int Size, void *pUsrData)
{
    return Cli_EBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::EBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_EBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ABRead(int Start, int Size, void *pUsrData)
{
    return Cli_ABRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ABWrite(int Start, int Size, void *pUsrData)
{
    return Cli_ABWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::TMRead(int Start, int Amount, void *pUsrData)
{
    return Cli_TMRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::TMWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_TMWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::CTRead(int Start, int Amount, void *pUsrData)
{
    return Cli_CTRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::CTWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_CTWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ListBlocks(PS7BlocksList pUsrData)
{
    return Cli_ListBlocks(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetAgBlockInfo(int BlockType, int BlockNum, PS7BlockInfo pUsrData)
{
    return Cli_GetAgBlockInfo(Client, BlockType, BlockNum, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetPgBlockInfo(void *pBlock, PS7BlockInfo pUsrData, int Size)
{
    return Cli_GetPgBlockInfo(Client, pBlock, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::ListBlocksOfType(int BlockType, TS7BlocksOfType *pUsrData, int *ItemsCount)
{
    return Cli_ListBlocksOfType(Client, BlockType, pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::Upload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_Upload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::FullUpload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_FullUpload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::Download(int BlockNum, void *pUsrData, int Size)
{
    return Cli_Download(Client, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::Delete(int BlockType, int BlockNum)
{
    return Cli_Delete(Client, BlockType, BlockNum);
}
//---------------------------------------------------------------------------
int TS7Client::DBGet(int DBNumber, void *pUsrData, int *Size)
{
    return Cli_DBGet(Client, DBNumber, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::DBFill(int DBNumber, int FillChar)
{
    return Cli_DBFill(Client, DBNumber, FillChar);
}
//---------------------------------------------------------------------------
int TS7Client::GetPlcDateTime(tm *DateTime)
{
    return Cli_GetPlcDateTime(Client, DateTime);
}
//---------------------------------------------------------------------------
int TS7Client::SetPlcDateTime(tm *DateTime)
{
    return Cli_SetPlcDateTime(Client, DateTime);
}
//---------------------------------------------------------------------------
int TS7Client::SetPlcSystemDateTime()
{
    return Cli_SetPlcSystemDateTime(Client);
}
//---------------------------------------------------------------------------
int TS7Client::GetOrderCode(PS7OrderCode pUsrData)
{
    return Cli_GetOrderCode(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetCpuInfo(PS7CpuInfo pUsrData)
{
    return Cli_GetCpuInfo(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetCpInfo(PS7CpInfo pUsrData)
{
    return Cli_GetCpInfo(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ReadSZL(int ID, int Index, PS7SZL pUsrData, int *Size)
{
    return Cli_ReadSZL(Client, ID, Index, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::ReadSZLList(PS7SZLList pUsrData, int *ItemsCount)
{
    return Cli_ReadSZLList(Client, pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::PlcHotStart()
{
    return Cli_PlcHotStart(Client);
}
//---------------------------------------------------------------------------
int TS7Client::PlcColdStart()
{
    return Cli_PlcColdStart(Client);
}
//---------------------------------------------------------------------------
int TS7Client::PlcStop()
{
    return Cli_PlcStop(Client);
}
//---------------------------------------------------------------------------
int TS7Client::CopyRamToRom(int Timeout)
{
    return Cli_CopyRamToRom(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::Compress(int Timeout)
{
    return Cli_Compress(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::GetProtection(PS7Protection pUsrData)
{
    return Cli_GetProtection(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::SetSessionPassword(char *Password)
{
    return Cli_SetSessionPassword(Client, Password);
}
//---------------------------------------------------------------------------
int TS7Client::ClearSessionPassword()
{
    return Cli_ClearSessionPassword(Client);
}
//---------------------------------------------------------------------------
int TS7Client::ExecTime()
{
    int Time;
    int Result = Cli_GetExecTime(Client, &Time);
    if (Result==0)
        return Time;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Client::LastError()
{
    int LastError;
    int Result =Cli_GetLastError(Client, &LastError);
    if (Result==0)
        return LastError;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Client::PDULength()
{
    int Requested, Negotiated;
    if (Cli_GetPduLength(Client, &Requested, &Negotiated)==0)
        return Negotiated;
    else
        return 0;
}
//---------------------------------------------------------------------------
int TS7Client::PDURequested()
{
    int Requested, Negotiated;
    if (Cli_GetPduLength(Client, &Requested, &Negotiated)==0)
        return Requested;
    else
        return 0;
}
//---------------------------------------------------------------------------
int TS7Client::PlcStatus()
{
    int Status;
    int Result = Cli_GetPlcStatus(Client, &Status);
    if (Result==0)
        return Status;
    else
        return Result;
}
//---------------------------------------------------------------------------
bool TS7Client::Connected()
{
    int ClientStatus;
    if (Cli_GetConnected(Client ,&ClientStatus)==0)
        return ClientStatus!=0;
    else
        return false;
}
//---------------------------------------------------------------------------
int TS7Client::SetAsCallback(pfn_CliCompletion pCompletion, void *usrPtr)
{
    return Cli_SetAsCallback(Client, pCompletion, usrPtr);
}
//---------------------------------------------------------------------------
bool TS7Client::CheckAsCompletion(int *opResult)
{
    return Cli_CheckAsCompletion(Client ,opResult)==JobComplete;
}
//---------------------------------------------------------------------------
int TS7Client::WaitAsCompletion(longword Timeout)
{
    return Cli_WaitAsCompletion(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::AsReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_AsReadArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsWriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_AsWriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsListBlocksOfType(int BlockType,  PS7BlocksOfType pUsrData, int *ItemsCount)
{
    return Cli_AsListBlocksOfType(Client, BlockType,  pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::AsReadSZL(int ID, int Index,  PS7SZL pUsrData, int *Size)
{
    return Cli_AsReadSZL(Client, ID, Index, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsReadSZLList(PS7SZLList pUsrData, int *ItemsCount)
{
    return Cli_AsReadSZLList(Client, pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::AsUpload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_AsUpload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsFullUpload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_AsFullUpload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsDownload(int BlockNum, void *pUsrData, int Size)
{
    return Cli_AsDownload(Client, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsCopyRamToRom(int Timeout)
{
    return Cli_AsCopyRamToRom(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::AsCompress(int Timeout)
{
    return Cli_AsCompress(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBRead(int DBNumber, int Start, int Size,void * pUsrData)
{
    DBTask.DBNumber=DBNumber;
    DBTask.Start=Start;
    DBTask.Size=Size;
    return Cli_AsDBRead(Client, DBNumber, Start, Size,pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBWrite(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_AsDBWrite(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsMBRead(int Start, int Size, void *pUsrData)
{
    return Cli_AsMBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsMBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_AsMBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsEBRead(int Start, int Size, void *pUsrData)
{
    return Cli_AsEBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsEBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_AsEBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsABRead(int Start, int Size, void *pUsrData)
{
    return Cli_AsABRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsABWrite(int Start, int Size, void *pUsrData)
{
    return Cli_AsABWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsTMRead(int Start, int Amount, void *pUsrData)
{
    return Cli_AsTMRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsTMWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_AsTMWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsCTRead(int Start, int Amount, void *pUsrData)
{
    return Cli_AsCTRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsCTWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_AsCTWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBGet(int DBNumber, void *pUsrData, int *Size)
{
    return Cli_AsDBGet(Client, DBNumber, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBFill(int DBNumber, int FillChar)
{
    return Cli_AsDBFill(Client, DBNumber, FillChar);
}
//==============================================================================
// SERVER
//==============================================================================
TS7Server::TS7Server()
{
    Server=Srv_Create();
}
//---------------------------------------------------------------------------
TS7Server::~TS7Server()
{
    Srv_Destroy(&Server);
}
//---------------------------------------------------------------------------
int TS7Server::Start()
{
    return Srv_Start(Server);
}
//---------------------------------------------------------------------------
int TS7Server::StartTo(const char *Address)
{
    return Srv_StartTo(Server, Address);
}
//---------------------------------------------------------------------------
int TS7Server::Stop()
{
    return Srv_Stop(Server);
}
//---------------------------------------------------------------------------
int TS7Server::GetParam(int ParamNumber, void *pValue)
{
    return Srv_GetParam(Server, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Server::SetParam(int ParamNumber, void *pValue)
{
    return Srv_SetParam(Server, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Server::SetEventsCallback(pfn_SrvCallBack PCallBack, void *UsrPtr)
{
    return Srv_SetEventsCallback(Server, PCallBack, UsrPtr);
}
//---------------------------------------------------------------------------
int TS7Server::SetReadEventsCallback(pfn_SrvCallBack PCallBack, void *UsrPtr)
{
    return Srv_SetReadEventsCallback(Server, PCallBack, UsrPtr);
}
//---------------------------------------------------------------------------
bool TS7Server::PickEvent(TSrvEvent *pEvent)
{
    int EvtReady;
    if (Srv_PickEvent(Server, pEvent, &EvtReady)==0)
        return EvtReady!=0;
    else
        return false;
}
//---------------------------------------------------------------------------
void TS7Server::ClearEvents()
{
    Srv_ClearEvents(Server);
}
//---------------------------------------------------------------------------
longword TS7Server::GetEventsMask()
{
    longword Mask;
    int Result = Srv_GetMask(Server, mkEvent, &Mask);
    if (Result==0)
        return Mask;
    else
        return 0;
}
//---------------------------------------------------------------------------
longword TS7Server::GetLogMask()
{
    longword Mask;
    int Result = Srv_GetMask(Server, mkLog, &Mask);
    if (Result==0)
        return Mask;
    else
        return 0;
}
//---------------------------------------------------------------------------
void TS7Server::SetEventsMask(longword Mask)
{
    Srv_SetMask(Server, mkEvent, Mask);
}
//---------------------------------------------------------------------------
void TS7Server::SetLogMask(longword Mask)
{
    Srv_SetMask(Server, mkLog, Mask);
}
//---------------------------------------------------------------------------
int TS7Server::RegisterArea(int AreaCode, word Index, void *pUsrData, word Size)
{
    return Srv_RegisterArea(Server, AreaCode, Index, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Server::UnregisterArea(int AreaCode, word Index)
{
    return Srv_UnregisterArea(Server, AreaCode, Index);
}
//---------------------------------------------------------------------------
int TS7Server::LockArea(int AreaCode, word Index)
{
    return Srv_LockArea(Server, AreaCode, Index);
}
//---------------------------------------------------------------------------
int TS7Server::UnlockArea(int AreaCode, word Index)
{
    return Srv_UnlockArea(Server, AreaCode, Index);
}
//---------------------------------------------------------------------------
int TS7Server::ServerStatus()
{
    int ServerStatus, CpuStatus, ClientsCount;
    int Result =Srv_GetStatus(Server, &ServerStatus, &CpuStatus, &ClientsCount);
    if (Result==0)
        return ServerStatus;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Server::GetCpuStatus()
{
    int ServerStatus, CpuStatus, ClientsCount;
    int Result =Srv_GetStatus(Server, &ServerStatus, &CpuStatus, &ClientsCount);
    if (Result==0)
        return CpuStatus;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Server::ClientsCount()
{
    int ServerStatus, CpuStatus, ClientsCount;
    int Result =Srv_GetStatus(Server, &ServerStatus, &CpuStatus, &ClientsCount);
    if (Result==0)
        return ClientsCount;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Server::SetCpuStatus(int Status)
{
    return Srv_SetCpuStatus(Server, Status);
}
//==============================================================================
// PARTNER
//==============================================================================
TS7Partner::TS7Partner(bool Active)
{
    Partner=Par_Create(int(Active));
}
//---------------------------------------------------------------------------
TS7Partner::~TS7Partner()
{
    Par_Destroy(&Partner);
}
//---------------------------------------------------------------------------
int TS7Partner::GetParam(int ParamNumber, void *pValue)
{
    return Par_GetParam(Partner, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Partner::SetParam(int ParamNumber, void *pValue)
{
    return Par_SetParam(Partner, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Partner::Start()
{
    return Par_Start(Partner);
}
//---------------------------------------------------------------------------
int TS7Partner::StartTo(const char *LocalAddress, const char *RemoteAddress, int LocalTSAP, int RemoteTSAP)
{
    return Par_StartTo(Partner, LocalAddress, RemoteAddress, LocalTSAP, RemoteTSAP);
}
//---------------------------------------------------------------------------
int TS7Partner::Stop()
{
    return Par_Stop(Partner);
}
//---------------------------------------------------------------------------
int TS7Partner::BSend(longword R_ID, void *pUsrData, int Size)
{
    return Par_BSend(Partner, R_ID, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Partner::AsBSend(longword R_ID, void *pUsrData, int Size)
{
    return Par_AsBSend(Partner, R_ID, pUsrData, Size);
}
//---------------------------------------------------------------------------
bool TS7Partner::CheckAsBSendCompletion(int *opResult)
{
    return Par_CheckAsBSendCompletion(Partner ,opResult)==JobComplete;
}
//---------------------------------------------------------------------------
int TS7Partner::WaitAsBSendCompletion(longword Timeout)
{
    return Par_WaitAsBSendCompletion(Partner, Timeout);
}
//---------------------------------------------------------------------------
int TS7Partner::SetSendCallback(pfn_ParSendCompletion pCompletion, void *usrPtr)
{
    return Par_SetSendCallback(Partner, pCompletion, usrPtr);
}
//---------------------------------------------------------------------------
int TS7Partner::BRecv(longword *R_ID, void *pUsrData, int *Size, longword Timeout)
{
    return Par_BRecv(Partner, R_ID, pUsrData, Size, Timeout);
}
//---------------------------------------------------------------------------
bool TS7Partner::CheckAsBRecvCompletion(int *opResult, longword *R_ID, void *pUsrData, int *Size)
{
    return Par_CheckAsBRecvCompletion(Partner, opResult, R_ID, pUsrData, Size) == JobComplete;
}
//---------------------------------------------------------------------------
int TS7Partner::SetRecvCallback(pfn_ParRecvCallBack pCallback, void *usrPtr)
{
    return Par_SetRecvCallback(Partner, pCallback, usrPtr);
}
//---------------------------------------------------------------------------
int TS7Partner::Status()
{
    int ParStatus;
    int Result = Par_GetStatus(Partner, &ParStatus);
    if (Result==0)
        return ParStatus;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Partner::LastError()
{
    int Error;
    int Result = Par_GetLastError(Partner, &Error);
    if (Result==0)
        return Error;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Partner::GetTimes(longword *SendTime, longword *RecvTime)
{
    return Par_GetTimes(Partner, SendTime, RecvTime);
}
//---------------------------------------------------------------------------
int TS7Partner::GetStats(longword *BytesSent, longword *BytesRecv, longword *ErrSend, longword *ErrRecv)
{
    return Par_GetStats(Partner, BytesSent, BytesRecv, ErrSend, ErrRecv);
}
//---------------------------------------------------------------------------
bool TS7Partner::Linked()
{
    return Status()==par_linked;
}
//==============================================================================
// Text routines
//==============================================================================
TextString CliErrorText(int Error)
{
    char text[TextLen];
    Cli_ErrorText(Error, text, TextLen);
    return TextString(text);
}
//---------------------------------------------------------------------------
TextString SrvErrorText(int Error)
{
    char text[TextLen];
    Srv_ErrorText(Error, text, TextLen);
    return TextString(text);
}
//---------------------------------------------------------------------------
TextString ParErrorText(int Error)
{
    char text[TextLen];
    Par_ErrorText(Error, text, TextLen);
    return TextString(text);
}
//---------------------------------------------------------------------------
TextString SrvEventText(TSrvEvent *Event)
{
    char text[TextLen];
    Srv_EventText(Event, text, TextLen);
    return TextString(text);
}

