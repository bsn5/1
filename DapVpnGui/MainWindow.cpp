#include <QMessageBox>
#include <QGraphicsView>
#include <QProgressDialog>
#include <QGraphicsBlurEffect>
#include <QStateMachine>
#include <QState>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QDesktopServices>

#ifdef DAP_SERVICE_CONNECT_TCP
#include <QTcpSocket>
#else
#include <QLocalSocket>
#endif

#include <QMenu>
#include <QLabel>
#include <QTimer>
#include <QFileInfo>
#include <QSystemTrayIcon>
#include <QToolButton>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

#include <QMainWindow>
#include <QScreen>
#include <QSettings>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif


#include "MainWindow.h"

#include "ServiceCtl.h"
#include "StatesHandler.h"
#include "StatsHandler.h"
#include "datalocal.h"
#include "schedule.h"

void MainWindow::updateUsrMsg()
{
    if (userMsgQueueIsEmpty() && !user_msg.isEmpty()) {
        dashboard_user_msg = user_msg.get_dashboard_user_MSG();
        qDebug() << "[set_user_msg_if_it_is] " << dashboard_user_msg;

        screen()->setVars("lbMessage","visible",true );
        screen()->setVars("spacerLabel","visible",true );
        screen()->setVars("btMessageClose","visible",true );
        screen()->setVars("lbMessage","text",dashboard_user_msg);
    } else if (userMsgQueueIsEmpty() && user_msg.isEmpty()) {
        screen()->setVars("lbMessage","visible",false );
        screen()->setVars("spacerLabel","visible",false );
        screen()->setVars("btMessageClose","visible",false );
        screen()->setVars("lbMessage","text","");
    }
}


void MainWindow::onBtMessageCloseClicked()
{
    qDebug() << "[user_click_close_msg]";
    dashboard_user_msg = "";
    updateUsrMsg();
}


void MainWindow::onLogout(){
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    //effect->setBlurRadius(20);
    effect->setBlurRadius(40);
    setGraphicsEffect(effect);
    screen()->setVars("graphicsView","visible",false);
    if(QMessageBox::question(
                this,
                "DiveVPN: Important information",
                "After that action the network will be unprotected, do you want to continue?"
                ) == QMessageBox::Yes) {
        emit sigBtDisconnect();
#ifdef Q_OS_ANDROID
        QtAndroid::androidActivity().callMethod<void>( "stopDapVpnServiceNative"
                                                       ,"()V" );
#endif
    }
    screen()->setVars("graphicsView","visible",true);
    setGraphicsEffect(0);
}

/**
 * @brief MainWindow::btLogin
 */
void MainWindow::onReqConnect(const DapServerInfo& dsi, QString a_user, QString a_ps)
{
    qDebug() << "[MW] btLogin()";
    setStatusText(""); // clear status text
    if (stateLoginConnecting->active() ) { //
        sendDisconnectionReq();
    } else {
        m_user = a_user;
        ServiceCtl::me().sendCmd(DapJsonCmd::generateCmd(
                                     DapJsonCommands::CONNECTION, {
                                         DapJsonParam(DapJsonParams::ADDRESS, dsi.address),
                                         DapJsonParam(DapJsonParams::PORT, dsi.port),
                                         DapJsonParam(DapJsonParams::USER, m_user),
                                         DapJsonParam(DapJsonParams::PASSWORD, a_ps)
                                     }));
    }
    emit sigBtConnect();
}

void MainWindow::sendDisconnectionReq() {
    auto cmd = DapJsonCmd::generateCmd(
                DapJsonCommands::CONNECTION,
    {DapJsonParam(DapJsonParams::DISCONNECT, true)});

    ServiceCtl::me().sendCmd(cmd);
}

void MainWindow::onExit()
{
    // sendCmd("disconnect");
    // ServiceCtl::me().sendCmd("disconnect");
    this->close();
}


void MainWindow::setStatusText(const QString &a_txt)
{
    screen()->setVars("lbMessage","text",a_txt);
}

void MainWindow::initTray()
{
    trayMenu = new QMenu(this);
    actionDisconnect = trayMenu->addAction(
                QIcon(":/pics/off_icon_active@3x.png"),
                "Disconnect", this, SLOT(onLogout()));
    trayMenu->addSeparator();

//    actionSettings = trayMenu->addAction(
//                QIcon(":/pics/settings_icon_active@3x.png"),"Settings",[=]{});

    trayMenu->addSeparator();
//    actionExit = trayMenu->addAction(QIcon(":/pics/close_icon@3x.png"),"Exit",
//                                     [=]{this->onExit();} );

    trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(QIcon(":/pics/logo_main@2x.png"));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(trayIcon, static_cast<void (QSystemTrayIcon::*)(QSystemTrayIcon::ActivationReason)>(&QSystemTrayIcon::activated),
            [=](QSystemTrayIcon::ActivationReason arg){
        if (arg == QSystemTrayIcon::Context) return;
        this->setVisible(!this->isVisible());
    });
}


/**
 * @brief MainWindow::initIndicators
 */
void MainWindow::initIndicators()
{
    // Authorization state
    siAuthorization= new  DapUiVpnStateIndicator(&sm,"cbAuthorized","lbAuthorizationText");
    siAuthorization->setUiLabelTextState(IndicatorState::True,tr("Authorized"));
    siAuthorization->setUiLabelTextState(IndicatorState::SwitchingToTrue,tr("Authorizing"));
    siAuthorization->setUiLabelTextState(IndicatorState::SwitchingToFalse,tr("Unauthorizing"));
    siAuthorization->setUiLabelTextState(IndicatorState::False,tr("Not Authorized"));
    siAll.append(siAuthorization);

    // Stream opened state
    siStream = new  DapUiVpnStateIndicator(&sm,"cbStream","lbStreamText");
    siStream->setUiLabelTextState(IndicatorState::True,tr("Stream Opened"));
    siStream->setUiLabelTextState(IndicatorState::SwitchingToTrue,tr("Stream Opening"));
    siStream->setUiLabelTextState(IndicatorState::SwitchingToFalse,tr("Stream Closing"));
    siStream->setUiLabelTextState(IndicatorState::False,tr("Stream Closed"));
    siAll.append(siStream);


    // Network configuration state
    siNetConf= new  DapUiVpnStateIndicator(&sm,"cbNetConf","lbNetConfText");
    siNetConf->setUiLabelTextState(IndicatorState::True,tr("Network Settings Present"));
    siNetConf->setUiLabelTextState(IndicatorState::SwitchingToTrue,tr("Network Settings Obtain"));
    siNetConf->setUiLabelTextState(IndicatorState::SwitchingToFalse,tr("Network Settings Unleash"));
    siNetConf->setUiLabelTextState(IndicatorState::False,tr("No Network Settings"));
    siAll.append(siNetConf);

    // Tunnel state
    siTunnel= new  DapUiVpnStateIndicator(&sm,"cbTunnel","lbTunnelText");
    siTunnel->setUiLabelTextState(IndicatorState::True,tr("Network Protected"));
    siTunnel->setUiLabelTextState(IndicatorState::SwitchingToTrue,tr("Network Protecting"));
    siTunnel->setUiLabelTextState(IndicatorState::SwitchingToFalse,tr("Network Unprotecting"));
    siTunnel->setUiLabelTextState(IndicatorState::False,tr("No Network Protection"));
    siAll.append(siTunnel);
}

/**
 * @brief MainWindow::initIndicatorsTransitions
 */
void MainWindow::initIndicatorsTransitions()
{
    //============ Indicator transitions ===========

    //----Authorization-----

    // Authorization ::False state
    siAuthorization->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigAuthorizing()),siAuthorization->state(IndicatorState::SwitchingToTrue));
    siAuthorization->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigAuthorized()),siAuthorization->state(IndicatorState::True));

    // Authorization ::FalseToTrue state
    siAuthorization->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigAuthorizeError()),siAuthorization->state(IndicatorState::False));
    siAuthorization->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigAuthorized()),siAuthorization->state(IndicatorState::True));
    siAuthorization->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigUnauthorized()),siAuthorization->state(IndicatorState::False));

    // Authorization ::TrueToFalse state
    siAuthorization->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigAuthorizing()),siAuthorization->state(IndicatorState::SwitchingToTrue));
    siAuthorization->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigAuthorized()),siAuthorization->state(IndicatorState::True));
    siAuthorization->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigUnauthorized()),siAuthorization->state(IndicatorState::False));

    // Authorization ::True state
    siAuthorization->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigAuthorizing()),siAuthorization->state(IndicatorState::SwitchingToTrue));
    siAuthorization->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigUnauthorized()),siAuthorization->state(IndicatorState::SwitchingToFalse));
    siAuthorization->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigAuthorizeError()),siAuthorization->state(IndicatorState::False));
    siAuthorization->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigUnauthorized()),siAuthorization->state(IndicatorState::False));

    //----Stream Opened-----

    // Stream ::False state
    siStream->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpening()),siStream->state(IndicatorState::SwitchingToTrue));
    siStream->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpened()),siStream->state(IndicatorState::True));
    siStream->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigStreamClosing()),siStream->state(IndicatorState::SwitchingToFalse));

    // Stream ::FalseToTrue state
    siStream->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigStreamError()),siStream->state(IndicatorState::False));
    siStream->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpened()),siStream->state(IndicatorState::True));
    siStream->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigStreamClosed()),siStream->state(IndicatorState::False));
    siStream->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigStreamClosing()),siStream->state(IndicatorState::SwitchingToFalse));

    // Stream ::TrueToFalse state
    siStream->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpening()),siStream->state(IndicatorState::SwitchingToTrue));
    siStream->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpened()),siStream->state(IndicatorState::True));
    siStream->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigStreamClosed()),siStream->state(IndicatorState::False));

    // Stream ::True state
    siStream->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpening()),siStream->state(IndicatorState::SwitchingToTrue));
    siStream->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigStreamClosing()),siStream->state(IndicatorState::SwitchingToFalse));
    siStream->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigStreamError()),siStream->state(IndicatorState::False));
    siStream->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigStreamClosed()),siStream->state(IndicatorState::False));

    //----NetConf Received-----

    // NetConf ::False state
    siNetConf->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigRequesting()),siNetConf->state(IndicatorState::SwitchingToTrue));
    siNetConf->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigTrue()),siNetConf->state(IndicatorState::True));

    // NetConf ::FalseToTrue state
    siNetConf->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigError()),siNetConf->state(IndicatorState::False));
    siNetConf->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigTrue()),siNetConf->state(IndicatorState::True));
    siNetConf->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigFalse()),siNetConf->state(IndicatorState::False));

    // NetConf ::TrueToFalse state
    siNetConf->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigTrue()),siNetConf->state(IndicatorState::True));
    siNetConf->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigFalse()),siNetConf->state(IndicatorState::False));

    // NetConf ::True state
    siNetConf->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigStreamOpening()),siNetConf->state(IndicatorState::SwitchingToTrue));

    siNetConf->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigError()),siNetConf->state(IndicatorState::False));
    siNetConf->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigNetConfigFalse()),siNetConf->state(IndicatorState::False));

    //----Tunnel -----

    // Tunnel ::False state
    siTunnel->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigTunnelCreating()),siTunnel->state(IndicatorState::SwitchingToTrue));
    siTunnel->state(IndicatorState::False)
            ->addTransition(statesHandler,SIGNAL(sigTunnelCreated()),siTunnel->state(IndicatorState::True));

    // Tunnel ::FalseToTrue state
    siTunnel->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigTunnelError()),siTunnel->state(IndicatorState::False));
    siTunnel->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigTunnelCreated()),siTunnel->state(IndicatorState::True));
    siTunnel->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigTunnelDestroyed()),siTunnel->state(IndicatorState::False));
    siTunnel->state(IndicatorState::SwitchingToTrue)
            ->addTransition(statesHandler,SIGNAL(sigTunnelDestroying()),siTunnel->state(IndicatorState::SwitchingToFalse));

    // Tunnel ::TrueToFalse state
    siTunnel->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigTunnelCreated()),siTunnel->state(IndicatorState::True));
    siTunnel->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigTunnelDestroyed()),siTunnel->state(IndicatorState::False));
    siTunnel->state(IndicatorState::SwitchingToFalse)
            ->addTransition(statesHandler,SIGNAL(sigTunnelCreating()),siTunnel->state(IndicatorState::SwitchingToTrue));

    // Tunnel ::True state
    siTunnel->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigTunnelCreating()),siTunnel->state(IndicatorState::SwitchingToTrue));

    siTunnel->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigTunnelError()),siTunnel->state(IndicatorState::False));
    siTunnel->state(IndicatorState::True)
            ->addTransition(statesHandler,SIGNAL(sigTunnelDestroyed()),siTunnel->state(IndicatorState::False));

}


MainWindow::MainWindow(QWidget *parent) :
    DapUiMainWindow(parent)
{

    qDebug() << "[MW] Initializing main window";
    statesHandler = ServiceCtl::me().getStatesHandleObj();

    // QWidget::setLayout: Attempting to set QLayout "" on MainWindow "", which already has a layout
    // Layout уже есть
    // https://stackoverflow.com/questions/10519006/qwidgetsetlayout-attempting-to-set-qlayout-on-widget-which-already-has
    //this->setLayout(new QVBoxLayout());

    this->setCentralWidget( pages = new QStackedWidget(this) );
    this->setGeometry(200,100,1280,800);
    pixSpotRed=QPixmap("qrc:/pics/point_red@3x.png");
    pixSpotGreen=QPixmap("qrc:/pics/point_green@3x.png");

    initTray();

    ////////////////////////////////////////////////////////////////////////////////
    sm.setChildMode(QState::ParallelStates);

    // states
    statesIndicators                = new QState(&sm);

    // User states
    statesUser                      = new QState(&sm);
    stateUserHistoryState           = new QHistoryState(statesUser);
    statesLogin                     = new QState(statesUser);
    statesDashboard                 = new QState(statesUser);
    stateExiting                    = new QState(statesUser);
    stateExit                       = new QFinalState(statesUser);

    // Login states
    stateLoginHistoryState          = new QHistoryState(statesLogin);
    stateLoginCtlConnecting         = new QState(statesLogin);
    stateLoginBegin                 = new QState(statesLogin);
    stateLoginConnecting            = new QState(statesLogin);

    // Dashboard states
    stateDashboardHistoryState      = new QHistoryState(statesDashboard);
    stateDashboardCtlConnecting     = new QState(statesDashboard);
    stateDashboardConnected         = new QState(statesDashboard);
    stateDashboardConnecting        = new QState(statesDashboard);
    stateDashboardDisconnecting     = new QState(statesDashboard);

    // Exiting states
    stateExiting                    = new QState(statesUser);
    stateExit                       = new QFinalState(statesUser);


    // Initial states
    statesUser->setInitialState(statesLogin);
    statesLogin->setInitialState(stateLoginCtlConnecting);
    statesDashboard->setInitialState(stateDashboardConnecting);

    // Login states slots
    connect(statesLogin, &QState::entered, [=]{
        qInfo() << "[MainWindow] States entered";
        this->newScreen(new ScreenLogin(this, pages));
        screen()->connectTo<QToolButton>("btRegister", &QToolButton::clicked,[=]{
            QDesktopServices::openUrl(QUrl("https://divevpn.com.ua/registration/"));
        });
        screen()->connectTo<QToolButton>("btRestore", &QToolButton::clicked,[=]{
            QDesktopServices::openUrl(QUrl("https://divevpn.com.ua/my-account/lost-password/"));
        });
        screen()->setVars("lbStatus","text","");
        screen()->setVars("btLogin","enabled",true);
        screen()->show();

        connectScreen<ScreenLogin>(&ScreenLogin::reqConnect, this,[=](DapServerInfo& dsi,
                                   const QString& a_user, const QString& a_ps) {
                onReqConnect(dsi, a_user, a_ps);
            }
        );


    });
    connect(statesLogin, &QState::exited, [=]{
        qInfo() << "[MainWindow]statesLogin exited";
    });

    connect(stateLoginCtlConnecting,&QState::entered,[=]{
        qInfo() << "[MainWindow] State LoginCtlConnecting entered";
        screen()->setVars("btLogin","enabled",false);
        screen()->setVars("cbServer","enabled",false);
        screen()->setVars("edPassword","enabled",false);
        screen()->setVars("edUsername","enabled",false);
        setStatusText(tr("Backend Reconnect..."));
        setEnabled(false);
        ServiceCtl::me().init();
    });

    connect(stateLoginCtlConnecting, &QState::exited,[=]{
        qInfo() << "[MainWindow] State LoginCtlConnecting exited";
        screen()->setVars("btLogin","enabled",true);
        screen()->setVars("cbServer","enabled",true);
        screen()->setVars("edPassword","enabled",true);
        screen()->setVars("edUsername","enabled",true);
        screen()->setVars("lbStatus","txt",tr("Connecting..."));
        setStatusText("");
        setEnabled(true);
    });

    connect(stateLoginBegin,&QState::entered, [=]{
        qInfo() << "[MainWindow] State Login Begin";
        screen()->setVars("btLogin","enabled",true);
        screen()->setVars("cbServer","enabled",true);
        screen()->setVars("edPassword","enabled",true);
        screen()->setVars("edUsername","enabled",true);
        screen()->setVars("btLogin","text",tr("Connect"));
        statesLogin->setInitialState(stateLoginBegin);
    });

    connect(stateLoginConnecting,&QState::entered, [=]{
        qInfo() << "[MainWindow] State Login Connecting";
        screen()->setVars("btLogin","text",tr("Stop"));
        screen()->setVars("cbServer","enabled",false);
        screen()->setVars("edPassword","enabled",false);
        screen()->setVars("edUsername","nabled",false);
        screen()->setVars("lbStatus","txt",tr("Connecting..."));
    });

    connect(statesDashboard, &QState::entered, [=]{
        qInfo() << "[MainWindow] State statesDashboard entered";
        newScreen(new ScreenDashboard(this, pages));

        screen()->connectTo("btDisconnect",   SIGNAL(clicked(bool)), this, SLOT(onLogout()));
        screen()->connectTo("btMessageClose", SIGNAL(clicked(bool)), this, SLOT(onBtMessageCloseClicked()));

        screen()->setVars("lbStreamText","text","");
        screen()->setVars("lbIpAddrText","text","");
        screen()->setVars("lbInterfaceText","text","");
        screen()->setVars("lbRouteText","text","");

        setStatusText(tr("Authorized as %1").arg(m_user));
        screen()->show();
    });


    // ============== State DashboardCtlConnecting ============
    connect(stateDashboardCtlConnecting,&QState::entered,[=]{
        screen()->setVars("btDisconnect","enabled",false);
        screen()->setVars("lbReceived","enabled",false);
        screen()->setVars("lbSent","enabled",false);
        screen()->setVars("lbTimeConnected", "enabled", false);
        setStatusText(tr("Backend Reconnect..."));
    });

    connect(stateDashboardCtlConnecting, &QState::exited,[=]{
        screen()->setVars("btLogin","enabled",true);
        screen()->setVars("lbReceived","enabled",true);
        screen()->setVars("lbSent","enabled",true);
        screen()->setVars("cbServer","enabled",true);
        screen()->setVars("edPassword","enabled",true);
        screen()->setVars("edUsername","enabled",true);
        screen()->setVars("lbStatus","txt",tr("Connecting..."));
        setStatusText("");
    });

    connect(statesDashboard, &QState::exited, [=]{
        qInfo() << "[MainWindow] States Dashboard exited";
    });


    connect(stateDashboardConnecting,&QState::entered,[=]{
        qInfo() << "[MainWindow] State Dashboard Connecting";
        screen()->setVars("btDisconnect","enabled",false);
        setStatusText("Connecting...");
    });

    connect(stateDashboardConnected,&QState::entered,[=]{
        qInfo() << "[MainWindow] State Dashboard Connected";
        screen()->setVars("lbReceived","enabled",true);
        screen()->setVars("lbSent","enabled",true);
        screen()->setVars("lbReceived","text", "Recieved: 0");
        screen()->setVars("lbSent","text", "Sent: 0");

        screen()->setVars("btDisconnect","checkable",false);
        screen()->setVars("btDisconnect","enabled",true);

        setStatusText("Connected");
    });


    // State Dashboard Disconnecting
    connect(stateDashboardDisconnecting, &QState::entered,[=]{
        qInfo() << "[MainWindow] State DashboardDisconnecting";
        screen()->setVars("btDisconnect","checkable",true);
        screen()->setVars("btDisconnect","checked",false);
        screen()->setVars("btDisconnect","enabled",false);
        screen()->setVars("lbStatus","text","Authorized, initializing the tunnel...");
        screen()->setVars("lbAuthorizedText","text",tr("User Authorized"));
        screen()->setVars("cbAuthorized","checked",true);
        setStatusText("Disconnecting...");
        sendDisconnectionReq();
    });

    // * ---> *CtlConnecting
    statesLogin->addTransition(&ServiceCtl::me(),SIGNAL(ctlDisconnected()),stateLoginCtlConnecting);
    statesDashboard->addTransition(&ServiceCtl::me(),SIGNAL(ctlDisconnected()),stateDashboardCtlConnecting);

    // *CtlConnecting ---> *
    stateLoginCtlConnecting->addTransition(&ServiceCtl::me(),SIGNAL(ctlConnected()),stateLoginBegin);
    stateDashboardCtlConnecting->addTransition(&ServiceCtl::me(), SIGNAL(ctlConnected()), stateDashboardConnected);

    // LoginBegin ---> LoginConnecting
    stateLoginBegin->addTransition(this,SIGNAL(sigBtConnect()),stateLoginConnecting);

    // LoginBegin ---> Dashboard
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigAuthorized()),statesDashboard);
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigStreamOpened()),statesDashboard);
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigNetConfigTrue()),statesDashboard);
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigTunnelCreated()),statesDashboard);

    // LoginConnecting ---> LoginBegin
    stateLoginConnecting->addTransition(this,SIGNAL(sigBtConnect()),stateLoginBegin);
    stateLoginConnecting->addTransition(statesHandler,SIGNAL(sigAuthorizeError()),stateLoginBegin);

    // LoginConnecting ---> Dashboard
    stateLoginConnecting->addTransition(statesHandler,SIGNAL(sigAuthorized()),statesDashboard);
    stateLoginConnecting->addTransition(statesHandler,SIGNAL(sigStreamOpened()),statesDashboard);
    stateLoginConnecting->addTransition(statesHandler,SIGNAL(sigNetConfigTrue()),statesDashboard);
    stateLoginConnecting->addTransition(statesHandler,SIGNAL(sigTunnelCreated()),statesDashboard);

    // Dashboard ---> DashboardDisconnecting
    // statesDashboard->addTransition(this,SIGNAL(sigBtDisconnect()),stateDashboardDisconnecting);

    // DashboardDisconnecting ---> Begin
    stateDashboardDisconnecting->addTransition(statesHandler,SIGNAL(sigUnauthorized()),stateLoginBegin);

    // Login Ctl Connecting ---> Login Begin
    stateLoginCtlConnecting->addTransition(&ServiceCtl::me(),SIGNAL(ctlConnected()),stateLoginBegin);

    // Login Begin ---> Login Connecting
    stateLoginBegin->addTransition(this,SIGNAL(sigBtConnect()),stateLoginConnecting );

    // Begin ---> Dashboard
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigAuthorized()),stateDashboardConnected);
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigStreamOpened()),stateDashboardConnected);
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigNetConfigTrue()),stateDashboardConnected);
    stateLoginBegin->addTransition(statesHandler,SIGNAL(sigTunnelCreated()),stateDashboardConnected);

    // DashboardConnecting --> DashboardConnected
    stateDashboardConnecting->addTransition(statesHandler ,SIGNAL(sigTunnelCreated()),stateDashboardConnected );

    // Dashboard ---> DashboardDisconnecting
    stateDashboardConnected->addTransition(this,SIGNAL(sigBtDisconnect()),stateDashboardDisconnecting);
    stateDashboardConnected->addTransition(statesHandler,SIGNAL(sigStreamClosed()) ,stateDashboardConnecting);
    stateDashboardConnected->addTransition(statesHandler,SIGNAL(sigTunnelDestroyed()) ,stateDashboardConnecting);
    stateDashboardConnected->addTransition(statesHandler,SIGNAL(sigNetConfigFalse()) ,stateDashboardConnecting);
    stateDashboardConnected->addTransition(statesHandler,SIGNAL(sigUnauthorized()) , stateDashboardConnecting);


    // DashboardDisconnecting ---> Begin
    stateDashboardDisconnecting->addTransition(statesHandler,SIGNAL(sigUnauthorized()),stateLoginBegin);

    initIndicators();
    initIndicatorsTransitions();

    sm.start();

    connect(ServiceCtl::me().getAuthErrorHandleObj(), &DapCmdAuthErrorHandler::errorMessage,
            [=](const QString&msg) { // connect error authorize message from service to status text
        setStatusText(msg);
    });

}
