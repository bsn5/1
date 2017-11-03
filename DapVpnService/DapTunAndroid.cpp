#include "DapTunAndroid.h"

DapTunAndroid::DapTunAndroid()
{

}


void DapTunAndroid::tunDeviceCreate()
{
    DapTunUnixAbstract::tunDeviceCreate();
    emit nativeCreateRequest();
}

void DapTunAndroid::tunDeviceDestroy()
{
    DapTunUnixAbstract::tunDeviceDestroy();
    emit nativeDestroyRequest();
}
