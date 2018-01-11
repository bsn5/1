#include <iostream>
#include <qdebug.h>
#include "tuntap.h"

static const char *
strerror_win32(DWORD errnum)
{
    switch (errnum)
    {
        case ERROR_GEN_FAILURE:
            return "General failure (ERROR_GEN_FAILURE)";
        case ERROR_IO_PENDING:
            return "I/O Operation in progress (ERROR_IO_PENDING)";
        case ERROR_IO_INCOMPLETE:
            return "I/O Operation in progress (WSA_IO_INCOMPLETE)";
        case WSAEINTR:
            return "Interrupted system call (WSAEINTR)";
        case WSAEBADF:
            return "Bad file number (WSAEBADF)";
        case WSAEACCES:
            return "Permission denied (WSAEACCES)";
        case WSAEFAULT:
            return "Bad address (WSAEFAULT)";
        case WSAEINVAL:
            return "Invalid argument (WSAEINVAL)";
        case WSAEMFILE:
            return "Too many open files (WSAEMFILE)";
        case WSAEWOULDBLOCK:
            return "Operation would block (WSAEWOULDBLOCK)";
        case WSAEINPROGRESS:
            return "Operation now in progress (WSAEINPROGRESS)";
        case WSAEALREADY:
            return "Operation already in progress (WSAEALREADY)";
        case WSAEDESTADDRREQ:
            return "Destination address required (WSAEDESTADDRREQ)";
        case WSAEMSGSIZE:
            return "Message too long (WSAEMSGSIZE)";
        case WSAEPROTOTYPE:
            return "Protocol wrong type for socket (WSAEPROTOTYPE)";
        case WSAENOPROTOOPT:
            return "Bad protocol option (WSAENOPROTOOPT)";
        case WSAEPROTONOSUPPORT:
            return "Protocol not supported (WSAEPROTONOSUPPORT)";
        case WSAESOCKTNOSUPPORT:
            return "Socket type not supported (WSAESOCKTNOSUPPORT)";
        case WSAEOPNOTSUPP:
            return "Operation not supported on socket (WSAEOPNOTSUPP)";
        case WSAEPFNOSUPPORT:
            return "Protocol family not supported (WSAEPFNOSUPPORT)";
        case WSAEAFNOSUPPORT:
            return "Address family not supported by protocol family (WSAEAFNOSUPPORT)";
        case WSAEADDRINUSE:
            return "Address already in use (WSAEADDRINUSE)";
        case WSAENETDOWN:
            return "Network is down (WSAENETDOWN)";
        case WSAENETUNREACH:
            return "Network is unreachable (WSAENETUNREACH)";
        case WSAENETRESET:
            return "Net dropped connection or reset (WSAENETRESET)";
        case WSAECONNABORTED:
            return "Software caused connection abort (WSAECONNABORTED)";
        case WSAECONNRESET:
            return "Connection reset by peer (WSAECONNRESET)";
        case WSAENOBUFS:
            return "No buffer space available (WSAENOBUFS)";
        case WSAEISCONN:
            return "Socket is already connected (WSAEISCONN)";
        case WSAENOTCONN:
            return "Socket is not connected (WSAENOTCONN)";
        case WSAETIMEDOUT:
            return "Connection timed out (WSAETIMEDOUT)";
        case WSAECONNREFUSED:
            return "Connection refused (WSAECONNREFUSED)";
        case WSAELOOP:
            return "Too many levels of symbolic links (WSAELOOP)";
        case WSAENAMETOOLONG:
            return "File name too long (WSAENAMETOOLONG)";
        case WSAEHOSTDOWN:
            return "Host is down (WSAEHOSTDOWN)";
        case WSAEHOSTUNREACH:
            return "No Route to Host (WSAEHOSTUNREACH)";
        case WSAENOTEMPTY:
            return "Directory not empty (WSAENOTEMPTY)";
        case WSAEPROCLIM:
            return "Too many processes (WSAEPROCLIM)";
        case WSAEUSERS:
            return "Too many users (WSAEUSERS)";
        case WSAEDQUOT:
            return "Disc Quota Exceeded (WSAEDQUOT)";
        case WSAESTALE:
            return "Stale NFS file handle (WSAESTALE)";
        case WSASYSNOTREADY:
            return "Network SubSystem is unavailable (WSASYSNOTREADY)";
        case WSAVERNOTSUPPORTED:
            return "WINSOCK DLL Version out of range (WSAVERNOTSUPPORTED)";
        case WSANOTINITIALISED:
            return "Successful WSASTARTUP not yet performed (WSANOTINITIALISED)";
        case WSAEREMOTE:
            return "Too many levels of remote in path (WSAEREMOTE)";
        case WSAHOST_NOT_FOUND:
            return "Host not found (WSAHOST_NOT_FOUND)";
        default:
            return "Unknown error";
    }
}

TunTap::TunTap()
{
    _fileDesriptorCounter = 100;
    _ifIndex[0] = _ifIndex[1] = -1;
    //pWinEvent = nullptr;
}

/**
 * @brief TunTap::ctl_code
 * @param deviceType
 * @param function
 * @param method
 * @param acess
 */
unsigned TunTap::ctl_code(unsigned deviceType, unsigned function, unsigned method, unsigned acess)
{
    return ((deviceType << 16) | (acess << 14) | (function << 2) | method);
}

/**
 * @brief TunTap::tap_control_code
 * @param request
 * @param method
 */
unsigned TunTap::tap_control_code(unsigned request, unsigned method)
{
    return ctl_code(FILE_DEVICE_UNKNOWN, request, method, FILE_ANY_ACCESS);
}

/**
 * @brief TunTap::_inet_to_a
 * @param in
 * @return
 */
QString TunTap::_inet_to_a(const in_addr &in)
{
    char _tmp[16]; // 255.255.255.255
    unsigned char *bytes = reinterpret_cast<unsigned char*>(&const_cast<in_addr&>(in));
    sprintf_s(_tmp, sizeof(_tmp), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return QString(_tmp);
}

QString TunTap::_inet_to_a2(DWORD in)
{
    char _tmp[16]; // 255.255.255.255
    unsigned char *bytes = (unsigned char*)(&in);
    sprintf_s(_tmp, sizeof(_tmp), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return QString(_tmp);
}

/**
 * @brief TunTap::_ipToNumber
 * @param str
 * @return
 */
unsigned long TunTap::_ipToNumber(const QString &str)
{
    QStringList digits = str.split('.');
    return (digits.at(3).toLong() << 24) | (digits.at(2).toLong() << 16)
            | (digits.at(1).toLong() << 8) | digits.at(0).toLong();
}


int TunTap::_getTunAdapterIndex()
{
    PIP_INTERFACE_INFO pInfo = nullptr;
    unsigned long ulOutBufLen = 0;

    unsigned long ulRetVal = 0;

    // Make an initial call to GetInterfaceInfo to get
    // the necessary size in the ulOutBufLen variable
    ulRetVal = GetInterfaceInfo(nullptr, &ulOutBufLen);
    if (ulRetVal == ERROR_INSUFFICIENT_BUFFER) {
        pInfo = (IP_INTERFACE_INFO *)::malloc(ulOutBufLen);
        if (pInfo == nullptr) {
            qDebug() << "Unable to allocate memory needed to call GetInterfaceInfo";
            return -1;
        }
    }
    ulRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen);
    if (ulRetVal == NO_ERROR) {
        for (int i=0; i < pInfo->NumAdapters; ++i) {
            QString strCompate = QString::fromWCharArray(pInfo->Adapter[i].Name);
            if (strCompate.indexOf(_tunWinId) != -1) {
                int tunNum = static_cast<int>(pInfo->Adapter[i].Index);
                qDebug () << "[TunTap] Tun Adapter number is " << tunNum;
                return tunNum;
            }
        }
    }
    ::free(pInfo);
    qDebug () << "Tun Adapter number NOT FOUND";
    return -1;
}

/**
 * @brief TunTap::overlappedIoInit
 * @param o
 * @param eventState
 */
void TunTap::overlappedIoInit(overlapped_io *o, bool eventState)
{
   memset(o, 0, sizeof(overlapped_io));
   o->overlapped.hEvent = CreateEvent(nullptr, true/*manual reset*/, eventState, nullptr);
   if (o->overlapped.hEvent == nullptr) {
       qDebug() << "Error: overlappedIoInit: CreateEvent failed";
   }
}

/**
 * @brief TunTap::getInstance
 * @return
 */
TunTap &TunTap::getInstance()
{
    static TunTap _this; return _this;
}

/**
 * @brief TunTap::makeTunTapDevice
 * @return a kernel descriptor of tuntap device
 */
int TunTap::makeTunTapDevice(QString &outTunName)
{
    _tunDevicesMap.clear();
    static WinSecurityParam secAttr;
    std::wstring devID = WinRegReader::getDeviceGuid();
    _tunWinId = QString(std::string(devID.begin(), devID.end()).c_str());
    _ifIndex[1] = _getTunAdapterIndex();
    std::wstring userModeDevice = L"\\\\.\\Global\\";
    userModeDevice += devID + L".tap";
    _fileHandle = CreateFile (
                userModeDevice.c_str(),
                MAXIMUM_ALLOWED,
                FILE_SHARE_READ|FILE_SHARE_WRITE,
                nullptr/*secAttr*/,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_SYSTEM|FILE_FLAG_OVERLAPPED,
                nullptr);

    if (_fileHandle == INVALID_HANDLE_VALUE) {
        std::wstring msg(L"Unable to open device file \"");
        msg.append(userModeDevice) + L'"';
        throw TunTapException(msg.c_str());
    }

    if (secAttr && secAttr.injectPermissions(_fileHandle))
        qDebug() << "[TunTap] Tap driver switch to maximum allow rules";

    int pStatus {1};
    unsigned long rLen;
    if (!DeviceIoControl ( _fileHandle,
                           tap_control_code(6, METHOD_BUFFERED),
                           &pStatus, sizeof(int),
                           &pStatus, sizeof(int),
                           &rLen, nullptr)) {
        std::wstring msg(L"Can not send commad to device \"");
        msg.append(userModeDevice) + L'"';
        throw TunTapException(msg.c_str());
    }


    int retDescr = _fileDesriptorCounter;
    _tunDevicesMap[_fileDesriptorCounter/*++*/] = _fileHandle;

    overlappedIoInit(&_tunDevicesMap[retDescr].reads, false);
    overlappedIoInit(&_tunDevicesMap[retDescr].writes, false);
    _tunDevicesMap[retDescr].rw_handle.read = _tunDevicesMap[retDescr].reads.overlapped.hEvent;
    _tunDevicesMap[retDescr].rw_handle.write = _tunDevicesMap[retDescr].writes.overlapped.hEvent;

    outTunName = _tunWinId + ".tap";

    unsigned long mtu;
    if (DeviceIoControl(_fileHandle, tap_control_code(3, METHOD_BUFFERED),
                        &mtu, sizeof(mtu),
                        &mtu, sizeof(mtu), &rLen, nullptr))
    {
        qDebug () << "[TunTap] TAP-Windows MTU =" << mtu;
    }
    _active = true;
    return retDescr;
}

/**
 * @brief TunTap::setAdress
 * @param ip
 * @param gw
 * @return
 */
int TunTap::setAdress(const QString &ip, const QString &gw, const QString &mask)
{
    qDebug() << "[TunTap] setAdress(" <<ip<<", "<<gw<<", "<<mask << " )";

    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize, dwRetVal;
    unsigned long rLen;
    pIPAddrTable = (MIB_IPADDRTABLE *) ::malloc(sizeof (MIB_IPADDRTABLE));
    if (pIPAddrTable) {
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            ::free(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *) ::malloc(dwSize);

        }
        if (pIPAddrTable == NULL) {
            qDebug("Memory allocation failed for GetIpAddrTable");
            assert(0);
        }
    }
    if ((dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) == NO_ERROR ) {
        struct in_addr ipAddr;
     //   qDebug() << "found this adapters and IPs:";
        for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
            int index = pIPAddrTable->table[i].dwIndex;
      //      qDebug() << "Adapter Index: " << index;
            ipAddr.S_un.S_addr = static_cast<unsigned long>(pIPAddrTable->table[i].dwAddr);
      //      qDebug() << "Adapter [" << index << "] Adress" << _inet_to_a(ipAddr);
            ipAddr.S_un.S_addr = static_cast<unsigned long>(pIPAddrTable->table[i].dwMask);
      //      qDebug() << "Adapter [" << index << "] Mask" << _inet_to_a(ipAddr);
            ipAddr.S_un.S_addr = static_cast<unsigned long>(pIPAddrTable->table[i].dwBCastAddr);
      //      qDebug() << "Adapter [" << index << "] BCastAddr" << _inet_to_a(ipAddr);
        }
     //   qDebug() << "Try to add ip params to tun adapter with Index=" << _ifIndex[1];
        /*ULONG*/ NTEContext = 0;
        ULONG NTEInstance = 0;
        long ipl = _ipToNumber(ip);
        long maskl = _ipToNumber(mask);
        if ((dwRetVal = AddIPAddress(ipl,
                                    maskl,
                                    _ifIndex[1],
                                    &NTEContext, &NTEInstance)) == NO_ERROR) {
            qDebug() << "[TunTap] ip address succefful assign to TUN";

        } else {
            qDebug() << "[TunTap] error with add ip address to TUN";
        }
    } else
        qDebug() << "TunTap:: GetIpAddrTable failed with error code " << dwRetVal;

    ::free(pIPAddrTable);

    QString _gw(gw);
    _gw.chop(1);
    _gw.append('0');
    int ip_addr1 = inet_addr(_gw.toLatin1().constData());
    int ip_addr2 = inet_addr(ip.toLatin1().constData());
    int ip_addr3 = inet_addr(mask.toLatin1().constData());
    int pTun[3];
    *pTun = ip_addr2; *(pTun+1) = ip_addr1; *(pTun+2) = ip_addr3;
    if (!DeviceIoControl ( _fileHandle,
                           tap_control_code(10, METHOD_BUFFERED),
                           pTun, 3*sizeof(int),
                           pTun, 3*sizeof(int),
                           &rLen, nullptr)) {
        qDebug() << "Can not send commad to device ip_nw="<<ip_addr1 << " ip_adapter="<<ip_addr2<<" mask="<<ip_addr3;
    }else
        qDebug() << "ip address succefful assign tu TUN driver ip_nw = "<<ip_addr1 << " ip_adapter="
                 <<ip_addr2<<" mask ="<<ip_addr3;
    return 0;
}

/**
 * @brief TunTap::getDefaultGateWay
 * @return default gateway adress.
 */
QString TunTap::getDefaultGateWay()
{
    PMIB_IPFORWARDTABLE pIpRouteTable {nullptr};
    pIpRouteTable = new(std::nothrow)MIB_IPFORWARDTABLE;
    if (pIpRouteTable) {
        unsigned long tableSize {0};
        if (GetIpForwardTable(pIpRouteTable, &tableSize, 0) ==
                ERROR_INSUFFICIENT_BUFFER) {
            delete pIpRouteTable;
            pIpRouteTable = new(std::nothrow)MIB_IPFORWARDTABLE[tableSize];
            if (!pIpRouteTable) {
                throw TunTapException(L"Can not allocate memory for get routing data");
            }
            struct in_addr ipAddr;
            if ((GetIpForwardTable(pIpRouteTable, &tableSize, 0)) == NO_ERROR) {
                for (unsigned long i=0; i<pIpRouteTable->dwNumEntries; ++i) {

                    ipAddr.S_un.S_addr = static_cast<unsigned long>(pIpRouteTable->table[i].dwForwardDest);
                    QString dest(inet_ntoa(ipAddr));

                    ipAddr.S_un.S_addr = static_cast<unsigned long>(pIpRouteTable->table[i].dwForwardMask);
                    QString mask(inet_ntoa(ipAddr));

                    ipAddr.S_un.S_addr = static_cast<unsigned long>(pIpRouteTable->table[i].dwForwardNextHop);
                    QString addr(inet_ntoa(ipAddr));
                    if ((dest == mask) && (dest == "0.0.0.0")) {
                        _defaultGateWay = pIpRouteTable->table[i];
                        //qDebug() << "++++++++++++++++++++ " << dest << " " << mask << " " << addr;
                        qDebug() << "[TunTap] default adapter index is " << pIpRouteTable->table[i].dwForwardIfIndex;
                        _ifIndex[0]=static_cast<int>(pIpRouteTable->table[i].dwForwardIfIndex);
                        delete [] pIpRouteTable;
                        return addr;
                    }
                }
            }
            delete [] pIpRouteTable;
        }
    }
    return QString();
}

/**
 * @brief TunTap::routeAdd
 * @param from - in adress
 * @param to - gateway adress
 * @return bool
 */
bool TunTap::defaultRouteDelete()
{
qDebug() << "defaultRouteDelete";
    //unsigned long status;
    // "route del default gw 192.168.0.1  // && route add default gw 10.0.0.1"
    if (DeleteIpForwardEntry(&_defaultGateWay) != ERROR_SUCCESS) {
        qDebug() << L"Could not delete default gateway";
        // no make throw here
        return false;
    }
    return true;
}

/**
 * @brief TunTap::routeAddFromTo
 * @param from
 * @param to
 * @return
 */
bool TunTap::makeRoute(AdapterType adapter, const QString &dest, const QString &gate, const QString &mask)
{
    if(dest.isEmpty())
    {
        qDebug() << "[TunTap] Error dest addr is empty!";
        return false;
    }
    unsigned long status;
    qDebug() << "TunTap::routeAddFromTo with from  = "
             << dest << ", to =" << gate;

    static int count = 0;
    if (!count) {
        ++count;
            qDebug() << "[TunTap] true-Default GW contains";
    } else {
         qDebug() << "[TunTap] current saved GW contains";
    }

    PMIB_IPFORWARDROW pIpWay = new MIB_IPFORWARDROW;
    memcpy(pIpWay, &_defaultGateWay, sizeof(MIB_IPFORWARDROW));
    pIpWay->dwForwardDest    = _ipToNumber(dest);   // 54.83.52.1
    pIpWay->dwForwardMask    = _ipToNumber(mask);   // mask
    pIpWay->dwForwardNextHop = _ipToNumber(gate);   // 192.168.0.1
    pIpWay->dwForwardIfIndex = (adapter==ETH)?_ifIndex[0]:_ifIndex[1];
    if (adapter == TUN) {
        pIpWay->dwForwardPolicy = 0;
        pIpWay->dwForwardType = 4;
        pIpWay->dwForwardProto = 3;
        pIpWay->dwForwardAge = 0;
        pIpWay->dwForwardNextHopAS = 0;
        pIpWay->dwForwardMetric1 = 1;
        pIpWay->dwForwardMetric2 = 0;
        pIpWay->dwForwardMetric3 = 0;
        pIpWay->dwForwardMetric4 = 0;
        pIpWay->dwForwardMetric5 = 0;
    }

    if ((status = CreateIpForwardEntry(pIpWay)) == NO_ERROR) { //SetIpForwardEntry
        qDebug() << "[TunTap] Created new gateway from [" << dest << "] to ["
                 << gate << "] with mask [" << mask << ']';
        delete pIpWay;
        return true;
    }
    delete pIpWay;
    qDebug() << "[TunTap] add new gateway from [" << dest << "] to ["
             << gate << "] with mask [" << mask << "] error. Error code == " << status;
    return false;
}


/**
 * @brief TunTap::getDefaultDest
 * @return
 */
QString TunTap::getDefaultDest()
{
    struct in_addr ipAddr;
    ipAddr.S_un.S_addr = static_cast<u_long>(_defaultGateWay.dwForwardDest);
    return _inet_to_a(ipAddr);
}

/**
 * @brief TunTap::getDefaulGateWay
 * @return
 */
QString TunTap::getDefaulGateWay()
{
    struct in_addr ipAddr;
    ipAddr.S_un.S_addr = static_cast<u_long>(_defaultGateWay.dwForwardNextHop);
    return _inet_to_a(ipAddr);
}

/**
 * @brief TunTap::getDefaultMask
 * @return
 */
QString TunTap::getDefaultMask()
{
    struct in_addr ipAddr;
    ipAddr.S_un.S_addr = static_cast<u_long>(_defaultGateWay.dwForwardMask);
    return _inet_to_a(ipAddr);
}

/**
 * @brief TunTap::getDefaultAdapterIndex
 * @return
 */
int TunTap::getDefaultAdapterIndex()
{
    return _ifIndex[0];
}

/**
 * @brief TunTap::getTunTapAdapterIndex
 * @return
 */
int TunTap::getTunTapAdapterIndex()
{
    return _ifIndex[1];
}

void TunTap::doCloseTun()
{
    _active = false;
}

void TunTap::closeTun()
{
    if (DeleteIPAddress(NTEContext) != NO_ERROR) {
        qDebug () << "+_           ERROR WITH UNASSIGN IP ADDRES FROM TUN ADAPTER";
    }
    CloseHandle(_fileHandle);
    TunDevice td = _tunDevicesMap.find(_fileDesriptorCounter).value();
    CloseHandle(td.writes.overlapped.hEvent);
    CloseHandle(td.reads.overlapped.hEvent);
}

TunTap::operator bool()
{
    return _active;
}

/**
 * @brief TunTap::getDefaultDNSList
 * @return
 */
QStringList TunTap::getDefaultDNSList()
{
    FIXED_INFO *pFixedInfo;
    unsigned long ulOutBufLen;
    unsigned long ulRetVal;

    QStringList ret;
    ulOutBufLen = sizeof(FIXED_INFO);
    pFixedInfo =  (FIXED_INFO*)::malloc(ulOutBufLen);

    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        ::free(pFixedInfo);
        pFixedInfo = (FIXED_INFO *)::malloc(ulOutBufLen);
        if (pFixedInfo == nullptr) {
            qDebug() << "Error allocating memory needed to call GetNetworkParams";
            return QStringList();
        }
    }

    if (ulRetVal = GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR) {

        ret.append(pFixedInfo->DnsServerList.IpAddress.String);

        auto pIPAddr = pFixedInfo->DnsServerList.Next;
        while (pIPAddr) {
            ret.append(pFixedInfo->DnsServerList.IpAddress.String);
            pIPAddr = pIPAddr->Next;
        }
    }
    else {
        qDebug() << "[TunTap] GetNetworkParams failed with error: " << ulRetVal;
        return QStringList();
    }

    ::free(pFixedInfo);
    return ret;
}

/**
 * @brief TunTap::getAdapterName
 * @param index
 * @return
 */
QString TunTap::getAdapterName(int index)
{
   if (index < 0) {
       qDebug() << "Error, use TunTap::getAdapterName after TunTap::getDefaultGateWay()";
       assert(0);
   }

   PIP_INTERFACE_INFO pInfo = nullptr;
   unsigned long ulOutBufLen = 0;
   unsigned long ulRetVal = 0;

   ulRetVal = GetInterfaceInfo(nullptr, &ulOutBufLen);
   if (ulRetVal == ERROR_INSUFFICIENT_BUFFER) {
       pInfo = (IP_INTERFACE_INFO *)::malloc(ulOutBufLen);
       if (pInfo == nullptr) {
           qDebug() << "Unable to allocate memory needed to call GetInterfaceInfo";
           return QString();
       }
   }
   ulRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen);
   if (ulRetVal == NO_ERROR) {
       for (int i=0; i < pInfo->NumAdapters; ++i) {
           QString strCompare = QString::fromWCharArray(pInfo->Adapter[i].Name);
           if (pInfo->Adapter[i].Index == index) {
               return strCompare.mid((int)strlen("\\DEVICE\\TCPIP_"));
           }
       }
   }
   ::free(pInfo);
   qDebug() << "ERROR in TunTap::getAdapterName - adapter name not found";
   return QString();
}

int TunTap::write_tun(int devID, unsigned char *buf, size_t len)
{
    int lo_len = -1;


    TunDevice   tunDevice;
    bool status;
    int  err;
    if (_tunDevicesMap.find(devID) !=_tunDevicesMap.end()) {
        tunDevice = _tunDevicesMap.find(devID).value();
    }
    else {
        qDebug() << "TunTap::write_tun: device not avaible";
        return -1;
    }

    status = WriteFile(
               tunDevice._fileHandle,
               buf,
               (DWORD)len,
               &tunDevice.writes.size,
               &tunDevice.writes.overlapped
               );

    lo_len = tunDevice.writes.size;

    if (!status && GetLastError() == ERROR_IO_PENDING) {
        WaitForSingleObject(tunDevice.writes.overlapped.hEvent, INFINITE);
        err = GetOverlappedResult(tunDevice._fileHandle, &tunDevice.writes.overlapped, (LPDWORD)&tunDevice.writes.size, FALSE);
        if (tunDevice.writes.size != len) {
            //qDebug()  << "TunTap::write_tunA error:: writed size != request write size";
            return -1;
        }
        if (err) return tunDevice.writes.size;

    }
    return lo_len;
}

int TunTap::read_tun(int devID, unsigned char *buf, size_t len)
{
    {
        int lo_len = -1;
        unsigned char *readBuffer = new unsigned char[len];


        TunDevice   tunDevice;
        bool status;
        if (_tunDevicesMap.find(devID) !=_tunDevicesMap.end()) {
            tunDevice = _tunDevicesMap.find(devID).value();
        }
        else {
            qDebug() << "TunTap::read_tun: device not avaible";
            return -1;
        }

        unsigned long retCode;
        status = ReadFile(
                   tunDevice._fileHandle,
                   readBuffer,
                   (DWORD)len,
                   &tunDevice.reads.size,
                   &tunDevice.reads.overlapped
                 );
        if (!status) {
            switch(retCode = GetLastError())
            {
            case ERROR_IO_PENDING:
                //qDebug() << "+_ read_tun ERROR_IO_PENDING";
                WaitForSingleObject(tunDevice.reads.overlapped.hEvent, INFINITE);
                retCode = GetOverlappedResult(tunDevice._fileHandle, &tunDevice.reads.overlapped, (LPDWORD)&tunDevice.reads.size, FALSE);
                if (retCode) {
                    memcpy((void*)buf, (void*)readBuffer, tunDevice.reads.size);
                    lo_len = tunDevice.reads.size;
                    goto end;
                }
            default:
                qDebug() << "+_ read_tun FATAL ERROR!";
                return -1;
            }
        }
        WaitForSingleObject(tunDevice.reads.overlapped.hEvent, INFINITE);
        retCode = GetOverlappedResult(tunDevice._fileHandle, &tunDevice.reads.overlapped, (LPDWORD)&tunDevice.reads.size, FALSE);
        memcpy((void*)buf, (void*)readBuffer, tunDevice.reads.size);
        //qDebug () << "+_ read_tun readet this data: [" << (char*)buf << "]";
        lo_len = tunDevice.reads.size;
    end:
        delete [] readBuffer;
            return lo_len;
    }
}

std::string GetLastErrorAsString()
{
    DWORD errorMessageID = GetLastError();
    if(errorMessageID == 0)
        return std::string("No error");

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}


