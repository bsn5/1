#ifndef DAPCMDHANDLERINTERFACE_H
#define DAPCMDHANDLERINTERFACE_H

#include <QJsonObject>

class DapCmdHandlerInterface {
public:
    virtual void handler(const QJsonObject* params) = 0;
    virtual ~DapCmdHandlerInterface() = 0;
};

#endif // DAPCMDHANDLERINTERFACE_H
