#ifndef SCREENLOGIN_H
#define SCREENLOGIN_H

#include <QHash>
#include "ScreenVpnAbstract.h"
#include "datalocal.h"

class ScreenLogin : public ScreenVpnAbstract
{
    Q_OBJECT

protected:
    void initUi(QWidget * a_w,ScreenRotation a_rotation);
public:
    ScreenLogin(QObject * a_parent, QStackedWidget * a_sw );
signals:
    void reqConnect(DapServerInfo&,QString,QString);
    void currentUpstreamNameChanged(QString);
};

#endif // SCREENLOGIN_H
