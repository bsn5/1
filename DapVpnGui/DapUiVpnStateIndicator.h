#ifndef DAPUIVPNSTATEINDICATOR_H
#define DAPUIVPNSTATEINDICATOR_H

#include <QObject>
#include <QState>
#include <QSharedPointer>
#include <QMap>
#include <QTimer>

class DapUiVpnStateIndicator: public QObject
{
    Q_OBJECT
public:
    enum IndicatorState{True, False,TrueToFalse,FalseToTrue};
    Q_ENUM(IndicatorState);
private:
    IndicatorState m_current;

    QMap<IndicatorState,QState*> m_state;
    QState * m_states; // Group of states o

    QString m_uiComboboxName;
    QString m_uiLabelName;

    QTimer tmrBlink;
    bool tmrBlinkOn;

    QMap<IndicatorState, QString> m_uiLabelTextState;
public:
    DapUiVpnStateIndicator(QState * a_statesParent,  const QString& a_uiComboboxName, const QString& a_uiLabelName);
    ~DapUiVpnStateIndicator();

    IndicatorState current(){ return m_current; }
    bool isTrue(){ return state(True)->active(); }
    const QString& uiComboboxName() { return m_uiComboboxName; }
    const QString& uiLabelName() { return m_uiLabelName; }

    const QString& uiLabelTextState(IndicatorState a_is) { return m_uiLabelTextState[a_is]; }
    void setUiLabelTextState(IndicatorState a_is, const QString& a_txt) { m_uiLabelTextState[a_is] = a_txt; }

    QState * state(IndicatorState a_is){ return m_state[a_is]; }
    QState * states() { return m_states; }
    void update();
};
#endif // DAPUIVPNDSTATEINDICATOR_H
