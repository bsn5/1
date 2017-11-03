#include <QtDebug>

#ifdef DAP_PLATFORM_DESKTOP
#include "ui_dashboard_desktop.h"
#include "ui_dashboard_desktop_big.h"
#include "ui_dashboard_desktop_small.h"
#else
#include "ui_dashboard_mobile_hor.h"
#include "ui_dashboard_mobile_hor_big.h"
#include "ui_dashboard_mobile_hor_small.h"
#include "ui_dashboard_mobile_ver.h"
#include "ui_dashboard_mobile_ver_big.h"
#include "ui_dashboard_mobile_ver_small.h"
#endif

#include "ScreenDashboard.h"
#include "datalocal.h"

/**
 * @brief DapUiScreenDashboard::initUiCommonAfter
 * @param a_w
 */
void ScreenDashboard::initUi(QWidget * a_w,ScreenRotation a_rotation)
{
    Q_UNUSED(a_rotation);
    fillUpstreams(a_w);

    QMap<QString, QString> serverKeys;
    QMap<QString, QString> countryFlags;

    // создаём сцену
    m_scene = new QGraphicsScene();
    QLabel *lbServerName = a_w->findChild<QLabel *>("lbServerName");
    QLabel *lbLocationFlag = a_w->findChild<QLabel *>("lbLocationFlag");
    QGraphicsView *graphicsView = a_w->findChild<QGraphicsView*>("graphicsView");
    QCheckBox *cbServersListVisible = a_w->findChild<QCheckBox*>("cbServersListVisible");
    graphicsView->setScene(m_scene);

    QRect rect = graphicsView->geometry();
    m_sceneHeight = rect.height();
    m_sceneWidth  = rect.width();

    graphicsView->setSceneRect(0,0,m_sceneWidth-3, m_sceneHeight-3);

    // список серверов
    QListWidget *cbUpstream = a_w->findChild<QListWidget*>("cbUpstream");
    cbUpstream->setVisible(false);

    connect(cbServersListVisible, &QCheckBox::stateChanged, [=](int st){
        cbUpstream->setVisible(st == 2);
    });

    for(const DapServerInfo& i: DataLocal::me()->servers()){
        QString keyStr = QString("%1:%2").arg(i.address).arg(i.port);
        QListWidgetItem *it = new QListWidgetItem(i.name);
        it->setIcon(QIcon(DataLocal::me()->locationToIcon(i.location)));
        // из-за идиотского устройства itemoв хранить данные прямо в них нельзя :(
        serverKeys[i.name] = keyStr;
        countryFlags[i.name] = DataLocal::me()->locationToIcon(i.location);
        cbUpstream->addItem(it);
    }

    connect(cbUpstream,&QListWidget::currentTextChanged ,[=]{
        m_currentUpstreamName = cbUpstream->currentItem()->data(0).toString();
        m_currentUpstreamAddr = serverKeys[m_currentUpstreamName];
        //lbServerName->setText(m_currentUpstreamName);
        //lbLocationFlag->setPixmap(QPixmap(countryFlags[m_currentUpstreamName]));
        emit currentUpstreamNameChanged(m_currentUpstreamName);
        emit reconnectSignal(m_currentUpstreamAddr, upstreamIp(m_currentUpstreamAddr));
    });

    //lbServerName->setText(m_currentUpstreamName);
    //lbLocationFlag->setPixmap(QPixmap(countryFlags[m_currentUpstreamName]));
}

void ScreenDashboard::authorzeIndicator(bool on)
{
    invokeMethods("lbStream_on", "setVisible", Q_ARG(bool, on));
    invokeMethods("lbStream_off", "setVisible", Q_ARG(bool, !on));
}

void ScreenDashboard::streamIndicator(bool on)
{
    invokeMethods("lbIpAddr_on", "setVisible", Q_ARG(bool, on));
    invokeMethods("lbIpAddr_off", "setVisible", Q_ARG(bool, !on));
}
void ScreenDashboard::netConfigIndicator(bool on)
{
    invokeMethods("lbRoute_on", "setVisible", Q_ARG(bool, on));
    invokeMethods("lbRoute_off", "setVisible", Q_ARG(bool, !on));
}
void ScreenDashboard::tunnelIndicator(bool on)
{
    invokeMethods("lbInterface_on", "setVisible", Q_ARG(bool, on));
    invokeMethods("lbInterface_off", "setVisible", Q_ARG(bool, !on));
}

/*
dus->invokeMethods("lbStream", "setVisible", Q_ARG(bool, true));
dus->invokeMethods("lbIpAddr", "setVisible", Q_ARG(bool, false));
dus->invokeMethods("lbInterface", "setVisible", Q_ARG(bool, false));
dus->invokeMethods("lbRoute", "setVisible", Q_ARG(bool, false));
*/

/**
 * @brief DapUiScreenDashboard::DapUiScreenDashboard
 * @param a_parent
 * @param a_sw
 */
ScreenDashboard::ScreenDashboard(QObject * a_parent, QStackedWidget * a_sw, QString upstreamName)
    :ScreenVpnAbstract(a_parent,a_sw)
{
    m_currentUpstreamName = upstreamName;
#ifdef DAP_PLATFORM_MOBILE
    create<Ui::DashboardHor,Ui::DashboardHorSmall,Ui::DashboardHorBig,
            Ui::DashboardVer,Ui::DashboardVerSmall,Ui::DashboardVerBig>();
#else
    create<Ui::Dashboard,Ui::DashboardSmall,Ui::DashboardBig>();
#endif
}
