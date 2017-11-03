#include "usrmsg.h"
#include <QDebug>

bool UsrMsg::isEmpty()
{
    bool result = elems.size() == 0;
    qDebug() << "[UsrMsg::isEmpty] " << result << ".";
    return result;
}


QString UsrMsg::get_dashboard_user_MSG()
{
    QString str = *elems.begin();
    qDebug() << "[UsrMsg::get_dashboard_user_MSG]" << str;
    elems.pop_front();
    return str;
}


int UsrMsg::size()
{
    int result = elems.size();
    qDebug() << "[UsrMsg::size] " << result;
    return result;
}
void UsrMsg::setMsq(QString str)
{
    qDebug() << "[UsrMsg::setMsq] " << str;
    elems.push_back(str);
}


UsrMsg::UsrMsg()
{

}
