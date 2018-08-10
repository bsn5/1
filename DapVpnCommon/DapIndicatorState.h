#ifndef DAPINDICATORSTATEE_H
#define DAPINDICATORSTATEE_H

#include <QObject>

class DapIndicatorState : public QObject
{
    Q_OBJECT
public:
    enum IndicatorState{True, False, SwitchingToFalse,
                        SwitchingToTrue, Error, ErrorAuth, ErrorNetwork};
    Q_ENUM(IndicatorState)

    explicit DapIndicatorState(QObject *parent = nullptr);

    static const QString& toString(IndicatorState i);
    static IndicatorState fromString(const QString& state);
private:
    static QMap<IndicatorState, QString> i2s;
};

#endif // DAPINDICATORSTATEE_H
