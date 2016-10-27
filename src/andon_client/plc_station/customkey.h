#define CUSTOMKEY_H
//#include <QHash>
#include <QString>
//#include <QVariant>
//#include "snap7.h"
//#include <qregexp.h>
#include <QRegExp>

/*
typedef struct
{
  QString       S7address;
  QString       Alias;
//  bool          Cyclical_flag; //true
  int           BitNumber;
//  PS7DataItem   ItemPointer;
  int   ItemNumber;
//  int           LastValue;
}TagCase;

//map<tagroot tag, QVariant last_value>;
*/

/*
bool isS7address(QString addrs)
{
    QRegExp isDBrx("DB[1-65535]\\.DB(X[0-65535]\\.[0-7]|B[0-65535]|W[0-65535]|D[0-65535])");
    QRegExp isMrx("M([1-65535]\\.[0-7]|B[0-65535]|W[0-65535]|D[0-65535])");
    //QStringList TagRegExp;
    //TagRegExp=QString("").split(",");
    //TagRegExp.at(i);
    //rx.cap(0);

    //M[1-65535]\\.[0-7]
    //MB[1-65535]
    //MW[1-65535]
    //MD[1-65535]
    //DB[1-65535]\\.DBX[0-65535]\\.[0-7]
    //DB[1-65535]\\.DBB[0-65535]
    //DB[1-65535]\\.DBW[0-65535]
    //DB[1-65535]\\.DBD[0-65535]

    //QRegExp rx("\\b");
    return isDBrx.exactMatch(addrs);
    //int pos = rx.search(addrs,pos);
}
*/
class CustomKey
{
    //constructor
    //destructor
public:
    CustomKey():m_S7address(""),m_Alias("")
    {

    }

    CustomKey(const CustomKey &other)
    {
        m_S7address=other.S7address();
        m_Alias=other.Alias();
    }

    CustomKey(const QString &TagKey1, const QString &TagKey2="")
    {
        QRegExp isDBrx("DB[1-65535]\\.DB(X[0-65535]\\.[0-7]|B[0-65535]|W[0-65535]|D[0-65535])");

        m_S7address="";
        m_Alias="";

        if (!TagKey1.isEmpty()) {
            if (!TagKey2.isEmpty()) {
                if (isDBrx.exactMatch(TagKey1)&&!isDBrx.exactMatch(TagKey2)) {
                    m_S7address=TagKey1;
                    m_Alias=TagKey2;
                }
                if (isDBrx.exactMatch(TagKey2)&&!isDBrx.exactMatch(TagKey1)) {
                    m_S7address=TagKey2;
                    m_Alias=TagKey1;
                }
            }
            else
                if (isDBrx.exactMatch(TagKey1))
                    m_S7address=TagKey1;
                else
                    m_Alias=TagKey1;
        }
    }

    bool isValid() const {
        return !(m_Alias.isEmpty() && m_S7address.isEmpty());
    }

    QString S7address() const {
        return m_S7address;
    }
    QString Alias() const {
        return m_Alias;
    }



    bool operator< (const CustomKey &other) const {
        return S7address() < other.S7address();
    }

    bool operator== (const CustomKey &other) const {
        return (S7address() == other.S7address() || Alias() == other.Alias());
    }

private:

    QString       m_S7address;
    QString       m_Alias;
//    int           m_BitNumber;
//    int           m_ItemNumber;
//    bool          m_IsArray;
//    bool          m_Cyclical_flag; //true && ItemNumber=<ItemCount-1


};

//inline uint qHash(const CustomKey &key, uint seed) {
//    return qHash(key.id(), seed);
//}




// CUSTOMKEY_H

