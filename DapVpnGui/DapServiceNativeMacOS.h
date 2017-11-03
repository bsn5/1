#ifndef DAPSERVICENATIVEMACOS_H
#define DAPSERVICENATIVEMACOS_H

#ifdef Q_OS_MACOS
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#endif

#include "DapServiceNativeAbstract.h"

class DapServiceNativeMacOS : public DapServiceNativeAbstract
{
public:
    DapServiceNativeMacOS();
    void checkInstallation() override;
    void restartService() override;
protected:
    bool rootExecute(const QString & a_cmd) override;
    void rootObtain() override;
    void rootLeft() override;
    void rootInstallService() override;
private:
//    AuthorizationRef authRef;
    bool isAuthRoot;
};

#endif // DAPSERVICENATIVEMACOS_H
