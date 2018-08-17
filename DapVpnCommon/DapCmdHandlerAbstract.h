#ifndef DAPCMDHANDLERINTERFACE_H
#define DAPCMDHANDLERINTERFACE_H

#include <QJsonObject>

class DapCmdHandlerAbstract {
public:
    virtual void handler(const QJsonObject* params) = 0;
    virtual ~DapCmdHandlerAbstract();
};

#endif // DAPCMDHANDLERINTERFACE_H
