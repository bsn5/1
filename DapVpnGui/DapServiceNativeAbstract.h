#ifndef DAPSERVICENATIVEABSTRACT_H
#define DAPSERVICENATIVEABSTRACT_H

#include <QString>

class DapServiceNativeAbstract
{
public:
    DapServiceNativeAbstract();
    virtual void checkInstallation() = 0;
    virtual void restartService() = 0;
    virtual void init();
protected:
    virtual bool rootExecute(const QString & a_cmd);
    virtual void rootObtain();
    virtual void rootLeft();
    virtual void rootInstallService();
};

#endif // DAPSERVICENATIVEABSTRACT_H
