#include <QString>
#ifndef MSGFORUSER_H
#define MSGFORUSER_H


class MsgForuser
{
    int m_time;
    QString msg;
public:
    MsgForuser();
    void setMSG(QString newMSG);
    QString getMSG();
};

#endif // MSGFORUSER_H
