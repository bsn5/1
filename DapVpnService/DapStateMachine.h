#ifndef DAPSTATEMACHINE_H
#define DAPSTATEMACHINE_H

#include <QObject>

class DapStateMachine : public QObject
{
    Q_OBJECT
public:
    explicit DapStateMachine(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DAPSTATEMACHINE_H