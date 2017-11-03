#include <QtDebug>
#include "DapSI.h"

/**
  * @details States thats possible to use as endpoint for the doActionFor() method
  * */
const QSet<DapSI::IndicatorState> DapSI::IS_ACTION_FOR_ALL{
    DapSI::True,DapSI::False
};

const QSet<DapSI::IndicatorState> DapSI::IS_ACTION_SWITCHING{
    DapSI::SwitchingToTrue,DapSI::SwitchingToFalse
};

/**
  * @var DapSI::IS_STATE_ALL
  * @details States thats used for the top level of states branch
  */
const QSet<DapSI::IndicatorState> DapSI::IS_STATE_ALL{
    DapSI::True,DapSI::False,DapSI::SwitchingToTrue,DapSI::SwitchingToFalse, DapSI::Error, DapSI::ErrorAuth, DapSI::ErrorNetwork
};

/**
  * @var DapSI::IS_STATE_TOP
  * @details top level of states
  */
const QSet<DapSI::IndicatorState> DapSI::IS_STATE_TOP{
    DapSI::True,DapSI::False,DapSI::SwitchingToTrue,DapSI::SwitchingToFalse, DapSI::Error
};

/**
  * @var DapSI::IS_SUBSTATES
  * @details Description of hierarchy of substates, detailes the IS_STATE_ALL set
  */
const QMap<DapSI::IndicatorState,QSet<DapSI::IndicatorState>> DapSI::IS_SUBSTATES{
    { DapSI::Error, {DapSI::ErrorAuth, DapSI::ErrorNetwork} }
};


/**
  * @var DapSI::IS_SUBSTATES
  * @details Set of states, accessible for the special signal that does uncondional transition to your state from any other
  */
const QSet<DapSI::IndicatorState> DapSI::IS_SIGNAL_ALL{
    DapSI::True,DapSI::False,DapSI::SwitchingToTrue,DapSI::SwitchingToFalse, DapSI::ErrorAuth, DapSI::ErrorNetwork, DapSI::Error
};

// Default state, thats shoud be clean
const DapSI::IndicatorState DapSI::IS_DEFAULT_STATE=DapSI::False;


/**
 * @brief StatesIndicator::StatesIndicator
 * @param a_name
 */
DapSI::DapSI(QState& a_parent, const QString& a_name)
    : QObject(nullptr), m_name(a_name), m_current(IS_DEFAULT_STATE), m_previous(IS_DEFAULT_STATE)
{

    m_states = new QState(&a_parent);
    for(auto i: IS_STATE_TOP){
        QState * s = m_state[i] = new QState(m_states);
        addState(i,s);
        for(auto j: IS_SUBSTATES[i])
            addState(j,m_state[j]=new QState(s) );
    }

    sb_child_state[SwitchingToFalse] = new DapSB(m_state[SwitchingToFalse]);
    sb_child_state[SwitchingToTrue] = new DapSB(m_state[SwitchingToTrue]);

    m_state[SwitchingToTrue]->setInitialState(state(SwitchingToTrue, DapSB::PATTERN_NORMAL));
    m_state[SwitchingToFalse]->setInitialState(state(SwitchingToFalse, DapSB::PATTERN_NORMAL));

    m_states->setInitialState(state(IS_DEFAULT_STATE));
}

/**
 * @brief DapSI::addState Add the new state to the list
 * @param a_is  Indicator enum value for the state
 * @param a_state The state itself
 */
void DapSI::addState(IndicatorState a_is, QState * a_state)
{
//    qDebug() << " == addState() "<< a_is;
    a_state->setObjectName(toString(a_is) );
    connect(a_state, &QState::entered, [=]{
//        qDebug() << "Entered in "<<a_is<<" for object "<<this->name();
        m_previous = m_current;
        m_current = a_is;
        emit currentChanged(a_is);
    } );

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
                if(IS_STATE_ALL.contains(j)){
//                    qDebug() << "Add transition from "<< p.first->objectName()<<"::"<<p.second<<" to state "<<j;
                    state(i)->addTransition(p.first,p.second,state(j));
                }else /// TODO process Error state and possible others with different blocks
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
    if(IS_ACTION_SWITCHING.contains(current()) ){
        qWarning() <<"Can't doActionFor() "<<a_is<<" because we're in switching action "<<current();
    }else if(current() != a_is){
        if(m_stateActionFor.contains(a_is) ){
            if(!m_stateActionFor[a_is].isEmpty()){
                qInfo()<< "==="<< name()<<"=== " <<"Do action for "<<a_is;
                for( auto p:m_stateActionFor[a_is])
                    QMetaObject::invokeMethod(p.first,p.second);
                return;
            }
        }
        qWarning() <<"No action for "<<a_is<<" state";
    }else
        qWarning() <<"doActionFor " <<a_is <<" state but we're already in this state!";
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
        {SwitchingToFalse,"true_to_false"},
        {SwitchingToTrue,"false_to_true"},
        {Error,"error"},
        {ErrorAuth,"error_auth"},
        {ErrorNetwork,"error_network"}
    };
    return i2s[i];
}

void DapSI::emitResponseWaitingSig()
{
    qDebug() << "emitResponseWaitingSig";
    if( sb_child_state.contains(m_current) )
        sb_child_state[m_current]->start_waiting();
    else
        qWarning() << "[DapSI] Error. Emit Waiting Signal, but no have object"
                      " Current state: " << toString(m_current);
}

QState * DapSI::state(DapSI::IndicatorState a_is, DapSB::SbState a_bs)
{
    if(a_is == SwitchingToFalse || a_is == SwitchingToTrue)
        return sb_child_state[a_is]->state(a_bs);

    return NULL;
}
