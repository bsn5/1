#ifndef DAPSB_H
#define DAPSB_H

#include <QObject>
#include <QState>
#include <QSet>
#include <QDebug>

class DapSB : public QObject
{
    Q_OBJECT
public:
    enum SbState { PATTERN_NORMAL, PATTERN_WAITING, PATTERN_CRITICAL };
    Q_ENUM(SbState)

    explicit DapSB(QState* parent);

    static const QSet<SbState> SB_STATE_ALL;

    QState * state(SbState a_is) { return m_state[a_is]; }

signals:
    void start_waiting();
    void end_waiting();

public slots:

private:
     const QString& toString(SbState i);
     QMap<SbState,QState*> m_state;
};

#endif // DAPSB_H
