#ifndef SIGUNIXHANDLER_H
#define SIGUNIXHANDLER_H

#include <QObject>

class SigUnixHandler : public QObject
{
    Q_OBJECT
    SigUnixHandler(QObject* obj = Q_NULLPTR);
    static void sig_handler_func(int sig_code);
    SigUnixHandler(const SigUnixHandler &) = delete;
public:
    static SigUnixHandler * getInstance() { static SigUnixHandler handler_obj; return &handler_obj; }
signals:
    void sigKill();
};

#endif // SIGUNIXHANDLER_H
