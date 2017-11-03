#ifndef SIGUNIXHANDLER_H
#define SIGUNIXHANDLER_H

#include <QObject>

class SigUnixHandler : public QObject
{
    SigUnixHandler();
    static void sig_handler_func(int sig_code);
    SigUnixHandler(const SigUnixHandler &) = delete;
public:
    static SigUnixHandler * getInstance() { static SigUnixHandler handler_obj; return &handler_obj; }
};

#endif // SIGUNIXHANDLER_H
