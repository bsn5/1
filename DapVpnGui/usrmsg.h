#ifndef USRMSG_H
#define USRMSG_H
#include <list>
#include <QString>

class UsrMsg
{
    std::list<QString> elems;
public:
    bool isEmpty();

    QString get_dashboard_user_MSG();

    int size();
    void setMsq(QString str);
    UsrMsg();
};

#endif // USRMSG_H
