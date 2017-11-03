#include "DapSB.h"

const QSet<DapSB::SbState> DapSB::SB_STATE_ALL {
   DapSB::PATTERN_NORMAL, DapSB::PATTERN_WAITING, DapSB::PATTERN_CRITICAL
};

const QString& DapSB::toString(SbState i)
{
    static QMap<SbState,QString> i2s {
        {PATTERN_NORMAL,"pattern_normal"},
        {PATTERN_WAITING,"pattern_waiting"},
        {PATTERN_CRITICAL,"pattern_critical"},
    };
    return i2s[i];
}

DapSB::DapSB(QState* parent) : QObject(nullptr)
{
    QState * s;
    for(auto i : SB_STATE_ALL)
    {
        s = new QState(parent);
        s->setObjectName(toString(i));
        connect(s, &QState::entered, [=] {
            qDebug() << "[DapSB] Enterned in state: " << s->objectName();
        } );
        m_state[i] = s;
    }

    m_state[PATTERN_NORMAL]->addTransition(this, &DapSB::start_waiting, m_state[PATTERN_WAITING]);
}
