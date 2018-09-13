#ifndef DAPTUNNATIVE_H
#define DAPTUNNATIVE_H

#include <QtGlobal>

#if defined(Q_OS_WINDOWS)
#include "DapTunWindows.h"
typedef class DapTunWindows DapTunNative;
#elif defined(Q_OS_ANDROID)
#include "DapTunAndroid.h"
typedef class DapTunAndroid DapTunNative;
#elif defined(Q_OS_MACOS)
#include "DapTunMac.h"
typedef class DapTunMac DapTunNative;
#elif defined(Q_OS_IOS)
#include "DapTunDarwin.h"
typedef class DapTunDarwin DapTunNative;
#elif defined(Q_OS_LINUX)
#include "DapTunLinux.h"
typedef class DapTunLinux DapTunNative;
#endif

#endif // DAPTUNNATIVE_H
