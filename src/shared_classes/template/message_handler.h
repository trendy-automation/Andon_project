#ifndef SINGLE_APPRUN
#define SINGLE_APPRUN


class SingleAppRun: public QObject
{
    Q_OBJECT
public:
    SingleAppRun(QObject *parent=0)
        : QObject(parent)
    {

    }


private:

};

#endif // SINGLE_APPRUN
