#ifndef DAPSTATEMACHINE_TEST_HPP
#define DAPSTATEMACHINE_TEST_HPP

#include <QTest>
#include "DapStateMachine.h"

class DapStateMachineTest : public QObject {
    Q_OBJECT
private slots:
    void someTest() {
        qDebug() << "SOME TEST";
    }
};

#endif // DAPSTATEMACHINE_TEST_HPP
