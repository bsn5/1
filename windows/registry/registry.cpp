#include "registry.h"
std::vector<std::wstring> WinRegReader::subKeys_;

void WinRegReader::readSubKeys(HKEY hKey)
{
    TCHAR    achKey[MAX_PATH];
    DWORD    cbName;
    TCHAR    achClass[MAX_PATH] = L"";
    DWORD    cchClassName {MAX_PATH};
    DWORD    cSubKeys {0};
    DWORD    cbMaxSubKey;
    DWORD    cchMaxClass;
    DWORD    cValues;
    DWORD    cchMaxValue;
    DWORD    cbMaxValueData;
    DWORD    cbSecurityDescriptor;
    FILETIME ftLastWriteTime;

    DWORD i, retCode;

    retCode = RegQueryInfoKey(
        hKey,                    // handle
        achClass,                // buffer for class name
        &cchClassName,           // size of class string
        NULL,                    // reserved
        &cSubKeys,               // number of subkeys
        &cbMaxSubKey,            // longest subkey size
        &cchMaxClass,            // longest class string
        &cValues,                // number of values for this key
        &cchMaxValue,            // longest value name
        &cbMaxValueData,         // longest value data
        &cbSecurityDescriptor,   // security descriptor
        &ftLastWriteTime);       // last write time

    if (cSubKeys) {
        subKeys_.clear();
        for (i=0; i<cSubKeys; i++) {
            cbName = MAX_PATH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey,
                     &cbName,
                     NULL,
                     NULL,
                     NULL,
                     &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS) {
                subKeys_.emplace_back(achKey);
            }
            else {
                throw TunTapException(L"Can't get subkeys");
            }
        }
    }
}

std::wstring WinRegReader::getDeviceGuid()
{
    std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}";
    HKEY networkInfo;
    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &networkInfo) == ERROR_SUCCESS ) {
        WinRegReader::readSubKeys(networkInfo);
    }
    regPath.append(L"\\");
    if (!WinRegReader::subKeys_.empty()) {
        for (auto &i : subKeys_) {
            std::wstring path = regPath+i;
            if (readRegistryKey(HKEY_LOCAL_MACHINE, path.c_str(), L"ComponentId") == L"tap0901") {
                return readRegistryKey(HKEY_LOCAL_MACHINE, path.c_str(), L"NetCfgInstanceId");
            }
        }
    }
    return L"N\\A";
}

std::string WinRegReader::getHumanName(std::wstring &guid)
{
    WinRegReader::getDeviceGuid();
    std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4d36e972-e325-11ce-bfc1-08002be10318}";
    std::wstring ret;
    regPath += L"\\" + guid + L"\\" + L"Connection";
    HKEY networkInfo;
    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &networkInfo) == ERROR_SUCCESS ){
        WinRegReader::readSubKeys(networkInfo);
    }
    if (!WinRegReader::subKeys_.empty()) {
        ret = readRegistryKey(HKEY_LOCAL_MACHINE, regPath.c_str(), L"Name");
        return std::string(ret.begin(), ret.end());
    }
    return "N\\A";

}

std::wstring WinRegReader::readRegistryKey(HKEY highKey, LPCWSTR regKey, LPCWSTR regPath)
{
    TCHAR Buffer[_MAX_PATH];
    DWORD BufSize = _MAX_PATH;
    DWORD RegKeySize;
    HKEY hKey;

    long lError = RegOpenKeyEx(highKey, regKey, 0, KEY_READ, &hKey);

    if (lError != ERROR_SUCCESS) {
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
            nullptr,
            lError,
            0,
            Buffer,
            _MAX_PATH,
            nullptr
        );
        PERR(false, Buffer);
        throw TunTapException(Buffer);
    }

    RegQueryValueEx(hKey, regPath, 0, &RegKeySize, reinterpret_cast<LPBYTE>(Buffer), &BufSize);

    switch (RegKeySize)
    {
        case REG_DWORD:
            return std::to_wstring(static_cast<DWORD>((*reinterpret_cast<DWORD*>(Buffer))));
        default:
        {
            auto BufferType = *Buffer;
            BufferType = BufferType;
            if (isSame<decltype(BufferType), WCHAR>()) {
                return Buffer;
            }
            else {
                wchar_t wstr[sizeof(Buffer)];
                std::mbstowcs(wstr, (char*)Buffer, sizeof(Buffer));
                return wstr;
            }
        }
    }
}

bool WinRegWriter::setDNS(QString sAdapterName, QString sDNS)
{
    HKEY hKey;
    QString strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\" + sAdapterName;

    if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     strKeyName.toStdWString().c_str(),
                     0,
                     KEY_WRITE,
                     &hKey) != ERROR_SUCCESS )
        return false;

    TCHAR Buffer[20];

    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, Buffer, sDNS.length() + 1, sDNS.toLatin1().data(), _TRUNCATE);

    convertedChars *= sizeof(TCHAR);

    RegSetValueEx(hKey, L"NameServer", 0, REG_SZ, reinterpret_cast<LPBYTE>(Buffer), convertedChars);

    RegCloseKey(hKey);

    return true;
}
