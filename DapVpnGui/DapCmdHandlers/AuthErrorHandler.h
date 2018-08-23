#ifndef DAPCMDAUTHERRORHANDLERR_H
#define DAPCMDAUTHERRORHANDLERR_H

#include <QObject>
#include <QDebug>
#include "DapCmdHandlerAbstract.h"

class DapCmdAuthErrorHandler : public QObject, public DapCmdHandlerAbstract
{
    Q_OBJECT
public:
    explicit DapCmdAuthErrorHandler(QObject *parent = nullptr);
    void handler(const QJsonObject* params);
signals:
    void errorMessage(const QString& message);
};

#endif // DAPCMDAUTHERRORHANDLERR_H
