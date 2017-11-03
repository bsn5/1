#include <QtDebug>
#include <QState>

#include "DapUiMainWindow.h"
#include "DapUiVpnStateIndicator.h"

/**
 * @brief DapUiVpnStateIndicator::DapUiVpnStateIndicator
 * @param a_statesParent
 * @param a_uiComboboxName
 * @param a_uiLabelName
 */
DapUiVpnStateIndicator::DapUiVpnStateIndicator(QState * a_statesParent,  const QString& a_uiComboboxName, const QString& a_uiLabelName)
    :QObject()
{
    tmrBlinkOn=false;
    m_current = False;
    m_states = new QState(a_statesParent);
    m_uiComboboxName = a_uiComboboxName;
    m_uiLabelName = a_uiLabelName;
    m_state[True] = new QState(states());
    m_state[False] = new QState(states());
    m_state[TrueToFalse] = new QState(states());
    m_state[FalseToTrue] = new QState(states());

    for (auto const &i: m_state.keys() ) {
         connect( state(i), &QState::entered, [=]{
                QString text = uiLabelTextState(i);
                m_current = i;
                if(text.size()>0)
                    DapUiMainWindow::getInstance()->setUiProp(uiLabelName(),"text",text);
                update();
           } );
    }
    tmrBlink.setInterval(500);
    connect(&tmrBlink, &QTimer::timeout,[=]{
        DapUiMainWindow::getInstance()->setUiProp(uiComboboxName(),"checked", (tmrBlinkOn = (!tmrBlinkOn)) );
    });
    m_states->setInitialState(m_state[False]);
}

void DapUiVpnStateIndicator::update()
{
    switch( current() ){
        case True: tmrBlink.stop(); DapUiMainWindow::getInstance()->setUiProp(uiComboboxName(),"checked",true); break;
        case False: tmrBlink.stop(); DapUiMainWindow::getInstance()->setUiProp(uiComboboxName(),"checked",false); break;
        case TrueToFalse: tmrBlink.start(); break;
        case FalseToTrue: tmrBlink.start(); break;
    }

}

/**
 * @brief DapUiVpnStateIndicator::~DapUiVpnStateIndicator
 */
DapUiVpnStateIndicator::~DapUiVpnStateIndicator()
{
    for(auto a:m_state)
        delete a;
    delete m_states;
}
