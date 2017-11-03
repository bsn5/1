#ifndef DAPSERVICENATIVEANDROID_H
#define DAPSERVICENATIVEANDROID_H

#include "DapServiceNativeAbstract.h"

class DapServiceNativeAndroid : public DapServiceNativeAbstract
{
public:
    DapServiceNativeAndroid();
    void restartService() override;
    void checkInstallation() override;
};

#endif // DAPSERVICENATIVEANDROID_H
