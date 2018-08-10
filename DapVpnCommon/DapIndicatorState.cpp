#include "DapIndicatorState.h"
#include <QMap>

DapIndicatorState::DapIndicatorState(QObject *parent)
    : QObject(parent) {

}

QMap<DapIndicatorState::IndicatorState, QString> DapIndicatorState::i2s{
    {True,"true"},
    {False,"false"},
    {SwitchingToFalse,"true_to_false"},
    {SwitchingToTrue,"false_to_true"},
    {Error,"error"},
    {ErrorAuth,"error_auth"},
    {ErrorNetwork,"error_network"}
};

DapIndicatorState::IndicatorState DapIndicatorState::fromString(const QString& state) {
    return DapIndicatorState::i2s.key(state);
}

const QString& DapIndicatorState::toString(IndicatorState i) {
    return i2s[i];
}
