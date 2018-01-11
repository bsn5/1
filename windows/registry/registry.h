#ifndef REGISTRY_H
#define REGISTRY_H

#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <QString>

#define PERR(bSuccess, msg) { if(!(bSuccess)) std::wcerr << __FILE__ << ":Error " << GetLastError() << " from " << msg << " on line " << __LINE__ << '\n'; }

struct TunTapException: public std::exception
{
    std::wstring _what;
    TunTapException(const wchar_t *s):_what(s){}
    const wchar_t *what() { return _what.c_str(); }
};

template <bool v> struct _is_same
{
    const bool value = v;
};

template <class T1, class T2> struct isSame: public _is_same<false>
{
    operator bool() { return value; }
};

template <class T> struct isSame<T, T>: public _is_same<true>
{
    operator bool() { return value; }
};

class WinRegReader
{
private:
    static std::vector<std::wstring> subKeys_;
    static std::wstring readRegistryKey(HKEY highKey, LPCWSTR regKey, LPCWSTR regPath);
    static void readSubKeys(HKEY hKey);
public:
    static std::wstring getDeviceGuid();
    static std::string getHumanName(std::wstring &guid);
};

class WinRegWriter
{
    WinRegWriter() { }
    WinRegWriter(const WinRegWriter&) = delete;
public:
    static WinRegWriter * getInstance() { static WinRegWriter wrw; return &wrw; }
    bool setDNS(QString adapterName, QString dns);
};

#endif
