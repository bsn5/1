#ifndef DAPSI_H
#define DAPSI_H

#include <QObject>
#include <QState>
#include <QSet>
#include <QMap>
#include <QPair>
#include <DapSB.h>
#include "DapIndicatorState.h"

class DapSI : public DapIndicatorState
{
    Q_OBJECT
public:
    typedef QPair<QObject*, const char*> SignalSender;

    static const QSet<IndicatorState> IS_SIGNAL_ALL;
    static const QSet<IndicatorState> IS_STATE_ALL;
    static const QSet<IndicatorState> IS_STATE_TOP;
    static const QSet<IndicatorState> IS_ACTION_FOR_ALL;
    static const QSet<IndicatorState> IS_ACTION_SWITCHING;
    static const QMap<IndicatorState,QSet<IndicatorState>> IS_SUBSTATES;
    static const IndicatorState IS_DEFAULT_STATE;
//    static const QMap<IndicatorState,IndicatorState> IS_SIGNAL_TO_STATE;
private:
    QString m_name;
    IndicatorState m_current;
    IndicatorState m_previous;

    QMap<IndicatorState, DapSB*> sb_child_state;

    QMap<IndicatorState,QState*> m_state;
    QMap<IndicatorState, QList<SignalSender> > m_stateSignals;
    QMap<IndicatorState, QList<SignalSender> > m_stateActionFor;

    QState * m_states; // Group of states o
    void addState(IndicatorState a_is,QState * a_state);
public:
    DapSI(QState& a_parent, const QString& a_name);
    ~DapSI();

    void addStateSignal(IndicatorState a_is,  QObject * a_sender, const char * a_signal);
    void addActionFor(IndicatorState a_is,  QObject * a_sender, const char * a_methodName);

    QState * state(IndicatorState a_is){ return m_state[a_is]; }
    QState * state(IndicatorState a_is, DapSB::SbState a_bs);

    const QString& name() { return m_name; }
    IndicatorState current() { return m_current; }
    IndicatorState previous() { return m_previous; }

signals:
    void currentChanged(IndicatorState);
public slots:
    void doActionFor(IndicatorState a_is);
    void addAllTransitions();
    void emitResponseWaitingSig();
};

#endif // DAPSI_H
