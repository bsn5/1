#include <QtDebug>
#include <QSettings>
#include <QComboBox>
#include "datalocal.h"
#include "ScreenVpnAbstract.h"

Q_DECLARE_METATYPE(DapServerInfo);

ScreenVpnAbstract::ScreenVpnAbstract(QObject * parent, QStackedWidget * a_sw)
        :DapUiScreen(parent,a_sw)
{

}

void ScreenVpnAbstract::fillUpstreams(QWidget * a_w)
{
    QComboBox * cbUpstream=a_w->findChild<QComboBox*>("cbUpstream");
    if(cbUpstream==nullptr){
        qWarning() << "[ScreenVpnAbstract] No cbUpstream component in "
                   << a_w->objectName() << " page";
        return;
    }

    int sepIndex = 0;
    for(const DapServerInfo& i: DataLocal::me()->servers()) {
        cbUpstream->addItem(QIcon(DataLocal::me()->locationToIconPath(i.location)),
                            i.name , QVariant::fromValue(i));
        if (sepIndex != 0)
            cbUpstream->insertSeparator(sepIndex*2-1);
        sepIndex++;
    }
}
