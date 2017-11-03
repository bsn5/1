#include <QtDebug>
#include "StatesIndicator.h"


const QSet<DapSI::IndicatorState> DapSI::IS_ACTION_FOR_ALL{
    DapSI::True,DapSI::False
};


const QSet<DapSI::IndicatorState> DapSI::IS_STATE_ALL{
    DapSI::True,DapSI::False,DapSI::FalseToTrue,DapSI::TrueToFalse, DapSI::Error
};

const QSet<DapSI::IndicatorState> DapSI::IS_SIGNAL_ALL{
    DapSI::True,DapSI::False,DapSI::FalseToTrue,DapSI::TrueToFalse, DapSI::Error
};

const DapSI::IndicatorState DapSI::IS_DEFAULT_STATE=DapSI::False;


/**
 * @brief StatesIndicator::StatesIndicator
 * @param a_name
 */
DapSI::DapSI(QState& a_parent, const QString& a_name)
    : QObject(nullptr), m_name(a_name), m_current(IS_DEFAULT_STATE), m_previous(IS_DEFAULT_STATE)
{

    m_states = new QState(&a_parent);
    for(auto i: IS_STATE_ALL){
        QState * s = new QState(m_states);
        s->setObjectName(toString(i) );
        connect(s, &QState::entered, [=]{
            m_previous = m_current;
            m_current = i;
            emit currentChanged(i);
        } );
        m_state[i] = s;
    }

    sb_child_state[TrueToFalse] = new DapSB(m_state[TrueToFalse]);
    sb_child_state[FalseToTrue] = new DapSB(m_state[FalseToTrue]);

    m_state[FalseToTrue]->setInitialState(state(FalseToTrue, DapSB::PATTERN_NORMAL));
    m_state[TrueToFalse]->setInitialState(state(TrueToFalse, DapSB::PATTERN_NORMAL));

    m_states->setInitialState(state(IS_DEFAULT_STATE));
}

/**
 * @brief StatesIndicator::~StatesIndicator
 */
DapSI::~DapSI()
{

}

/**
 * @brief addStateSignal
 * @param a_is
 * @param a_sender
 * @param a_signal
 */
void DapSI::addStateSignal(IndicatorState a_is,  QObject * a_sender, const char * a_signal)
{
    if(IS_SIGNAL_ALL.contains(a_is))
        m_stateSignals[a_is].append(SignalSender(a_sender,a_signal));
    else
        qWarning() <<"addStateSignal() with "<< a_is<<" state thats not listed for StateSignal states";
}

/**
 * @brief addActionFor
 * @param a_is
 * @param a_sender
 * @param a_methodName
 */
void DapSI::addActionFor(IndicatorState a_is,  QObject * a_sender, const char * a_methodName)
{
    if(IS_ACTION_FOR_ALL.contains(a_is)){
        qDebug() << "==="<< name()<<"=== "<< "Added action for indicator state "<<a_is;
        m_stateActionFor[a_is].append(SignalSender(a_sender,a_methodName));
    }else
        qWarning() <<"Trying to add action for "<< a_is<<" state thats not listed for ActionFor states";
}


/**
 * @brief StatesIndicator::updateTransitions
 */
void DapSI::addAllTransitions()
{
    for(auto i: IS_STATE_ALL){
        for(auto j: IS_SIGNAL_ALL ){
            for(auto p: m_stateSignals[j] ){
                if(IS_STATE_ALL.contains(j))
                    state(i)->addTransition(p.first,p.second,state(j));
                else /// TODO process Error state and possible others with different blocks
                    state(i)->addTransition(p.first,p.second,state(IS_DEFAULT_STATE) );
            }
        }
    }
}

/**
 * @brief StatesIndicator::doActionFor
 * @param a_is
 */
void DapSI::doActionFor(IndicatorState a_is)
{
    if(m_stateActionFor.contains(a_is) ){
        if(!m_stateActionFor[a_is].isEmpty()){
            qInfo()<< "==="<< name()<<"=== " <<"Do action for "<<a_is;
            for( auto p:m_stateActionFor[a_is])
                QMetaObject::invokeMethod(p.first,p.second);
            return;
        }
    }

    qWarning() <<"No action for "<<a_is<<" state";
}

/**
 * @brief StatesIndicator::toString
 * @param i
 * @return
 */
const QString& DapSI::toString(IndicatorState i)
{
    static QMap<IndicatorState,QString> i2s{
        {True,"true"},
        {False,"false"},
        {TrueToFalse,"true_to_false"},
        {FalseToTrue,"false_to_true"},
        {Error,"error"},
    };
    return i2s[i];
}

void DapSI::emitResponseWaitingSig()
{
    qDebug() << "emitResponseWaitingSig";
    if( sb_child_state.contains(m_current) )
        sb_child_state[m_current]->start_waiting();
    else
        qWarning() << "[DapSI] Error Emit Waiting Signal, but no have object"
                      " Current state: " << toString(m_current);
}

QState * DapSI::state(DapSI::IndicatorState a_is, DapSB::SbState a_bs)
{
    if(a_is == TrueToFalse || a_is == FalseToTrue)
        return sb_child_state[a_is]->state(a_bs);

    return NULL;
}
