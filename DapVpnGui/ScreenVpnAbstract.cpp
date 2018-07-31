#include <QtDebug>
#include <QSettings>
#include <QComboBox>
#include "datalocal.h"
#include "ScreenVpnAbstract.h"

ScreenVpnAbstract::ScreenVpnAbstract(QObject * parent, QStackedWidget * a_sw)
        :DapUiScreen(parent,a_sw)
{

}

void ScreenVpnAbstract::fillUpstreams(QWidget * a_w)
{
    QComboBox * cbUpstream=a_w->findChild<QComboBox*>("cbUpstream");
    if(cbUpstream==nullptr){
        qWarning() << "[ScreenVpnAbstract] No cbUpstream component in "<<a_w->objectName()<<" page";
        return;
    }


    int sepIndex = 0;
    for(const DapServerInfo& i: DataLocal::me()->servers()){
        QString keyStr = QString("%1:%2").arg(i.address).arg(i.port);
        QVariant dataStr = keyStr;
        cbUpstream->addItem(QIcon(DataLocal::me()->locationToIconPath(i.location)), i.name ,dataStr);

        if (sepIndex != 0)
            cbUpstream->insertSeparator(sepIndex*2-1);

        m_upstreamIp[keyStr] = i.ip;
        sepIndex++;
    }
}
