#ifndef SCREENVPNABSTRACT_H
#define SCREENVPNABSTRACT_H

#include "DapUiScreen.h"
#include "DapUiDialog.h"

class ScreenVpnAbstract : public DapUiScreen
{
public:
    ScreenVpnAbstract(QObject * parent, QStackedWidget * a_sw);
    QString upstreamIp(const QString& a_addr){ return m_upstreamIp[a_addr]; }
    QString currentUpstreamAddr() { return m_currentUpstreamAddr; }
    QString currentUpstreamName() { return m_currentUpstreamName;}
protected:
    void fillUpstreams(QWidget * a_w);
    QString m_currentUpstreamAddr;
    QString m_currentUpstreamName;
private:
    DapUiDialog * dlgAbout, * dlgSettings, *dlgHelp;
    QHash<QString,QString> m_upstreamIp;
};

#endif // SCREENVPNABSTRACT_H
