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

#include "datalocal.h"
#include "schedule.h"

void MainWindow::updateUsrMsg()
{
    if (userMsgQueueIsEmpty() && !user_msg.isEmpty()) {
        dashboard_user_msg = user_msg.get_dashboard_user_MSG();
        qDebug() << "[set_user_msg_if_it_is] " << dashboard_user_msg;

        dus->setVars("lbMessage","visible",true );
        dus->setVars("spacerLabel","visible",true );
        dus->setVars("btMessageClose","visible",true );
        dus->setVars("lbMessage","text",dashboard_user_msg);
    } else if (userMsgQueueIsEmpty() && user_msg.isEmpty()) {
        dus->setVars("lbMessage","visible",false );
        dus->setVars("spacerLabel","visible",false );
        dus->setVars("btMessageClose","visible",false );
        dus->setVars("lbMessage","text","");
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
    dus->setVars("graphicsView","visible",false);
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
    dus->setVars("graphicsView","visible",true);
    setGraphicsEffect(0);
}



void MainWindow::sendUpstreamsToServer()
{
//    for(const DapServerInfo& i: DataLocal::me()->servers()){
//        ServiceCtl::me().sendCmd(QString("addServerToList %1 %2")
//            .arg(QString("%1:%2").arg(i.address).arg(i.port))
//            .arg(i.ip));
//    }

//#ifdef  QT_DEBUG
//    ServiceCtl::me().sendCmd("addServerToList testing.divevpn.com:8003 62.210.73.95");
//    ServiceCtl::me().sendCmd("addServerToList dev1.demlabs.net:8001 62.210.73.95");
//    ServiceCtl::me().sendCmd("addServerToList dev2.demlabs.net:8002 62.210.73.95");
//    ServiceCtl::me().sendCmd("addServerToList 192.168.0.104:8002 192.168.0.104"); //TODO: IVAN
//   // ServiceCtl::me().sendCmd("addServerToList 127.0.0.1:8002 127.0.0.1"); //TODO: IVAN
//#endif
}

/**
 * @brief MainWindow::btLogin
 */
void MainWindow::onReqConnect(const DapServerInfo& dsi, QString a_user, QString a_ps)
{
    qDebug() << "[MW] btLogin()";
    if (stateLoginConnecting->active() ) { //
        ServiceCtl::me().sendCmd("disconnect");
    } else {
        m_user = a_user;

        ServiceCtl::me().sendCmd(QString("connect %1 %2 %3 %4")
            .arg(dsi.address).arg(dsi.port).arg(m_user).arg(a_ps));
        sendUpstreamsToServer();
    }
    emit sigBtConnect();
}

void MainWindow::raiseErrorDialog(const QString &text)
{
    qDebug() << "Error: '" << text << "'";
    QMessageBox::information(this, "Error occured",text);
}


void MainWindow::raiseNotifyDialog(const QString &text)
{
    qDebug() << "Notify: '" << text << "'";
    QMessageBox::information(this, "Error occured",text);
}


void MainWindow::raiseProgressDialog(const QString &text)
{
    qDebug() << "Progress dialog rise";
    (void) text;
}


void MainWindow::hideProgressDialog()
{
    qDebug() << "Progress dialog hide";
    if (!dlgProgress) return;

    dlgProgress->hide();
    delete dlgProgress;
    dlgProgress = Q_NULLPTR;
}


void MainWindow::updateProgress(int value)
{
    Q_UNUSED(value);
    hideProgressDialog();
}

bool stringToBool (QString str) {return str == "true";}


void MainWindow::onExit()
{
    ServiceCtl::me().sendCmd("disconnect");
    this->close();
}


void MainWindow::setStatusText(const QString &a_txt)
{
    dus->setVars("lbMessage","text",a_txt);
}

void MainWindow::initTray()
{
    trayMenu = new QMenu(this);
    actionDisconnect = trayMenu->addAction(
        QIcon(":/pics/off_icon_active@3x.png"),
        "Disconnect", this, SLOT(onLogout()));
    trayMenu->addSeparator();
    actionSettings = trayMenu->addAction(
        QIcon(":/pics/settings_icon_active@3x.png"),"Settings",[=]{});
    trayMenu->addSeparator();
    actionExit = trayMenu->addAction(QIcon(":/pics/close_icon@3x.png"),"Exit",
        [=]{this->onExit();} );

    trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(QIcon(":/pics/logo-main@2x.png"));
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
    siAuthorization= new  DapUiVpnStateIndicator(&sm,"cbAuthorized","lbAuthorizedText");
    siAuthorization->setUiLabelTextState(DapUiVpnStateIndicator::True,tr("Authorized"));
    siAuthorization->setUiLabelTextState(DapUiVpnStateIndicator::FalseToTrue,tr("Authorizing"));
    siAuthorization->setUiLabelTextState(DapUiVpnStateIndicator::TrueToFalse,tr("Unauthorizing"));
    siAuthorization->setUiLabelTextState(DapUiVpnStateIndicator::False,tr("Not Authorized"));
    siAll.append(siAuthorization);

    // Stream opened state
    siStream = new  DapUiVpnStateIndicator(&sm,"cbStream","lbStreamText");
    siStream->setUiLabelTextState(DapUiVpnStateIndicator::True,tr("Stream Opened"));
    siStream->setUiLabelTextState(DapUiVpnStateIndicator::FalseToTrue,tr("Stream Opening"));
    siStream->setUiLabelTextState(DapUiVpnStateIndicator::TrueToFalse,tr("Stream Closing"));
    siStream->setUiLabelTextState(DapUiVpnStateIndicator::False,tr("Stream Closed"));
    siAll.append(siStream);


    // Network configuration state
    siNetConf= new  DapUiVpnStateIndicator(&sm,"cbNetConf","lbNetConfText");
    siNetConf->setUiLabelTextState(DapUiVpnStateIndicator::True,tr("Network Settings Present"));
    siNetConf->setUiLabelTextState(DapUiVpnStateIndicator::FalseToTrue,tr("Network Settings Obtain"));
    siNetConf->setUiLabelTextState(DapUiVpnStateIndicator::TrueToFalse,tr("Network Settings Unleash"));
    siNetConf->setUiLabelTextState(DapUiVpnStateIndicator::False,tr("No Network Settings"));
    siAll.append(siNetConf);

    // Tunnel state
    siTunnel= new  DapUiVpnStateIndicator(&sm,"cbTunnel","lbTunnelText");
    siTunnel->setUiLabelTextState(DapUiVpnStateIndicator::True,tr("Network Protected"));
    siTunnel->setUiLabelTextState(DapUiVpnStateIndicator::FalseToTrue,tr("Network Protecting"));
    siTunnel->setUiLabelTextState(DapUiVpnStateIndicator::TrueToFalse,tr("Network Unprotecting"));
    siTunnel->setUiLabelTextState(DapUiVpnStateIndicator::False,tr("No Network Protection"));
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
    siAuthorization->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorizing()),siAuthorization->state(DapUiVpnStateIndicator::FalseToTrue));
    siAuthorization->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorized()),siAuthorization->state(DapUiVpnStateIndicator::True));

    // Authorization ::FalseToTrue state
    siAuthorization->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorizeError()),siAuthorization->state(DapUiVpnStateIndicator::False));
    siAuthorization->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorized()),siAuthorization->state(DapUiVpnStateIndicator::True));
    siAuthorization->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorized()),siAuthorization->state(DapUiVpnStateIndicator::False));

    // Authorization ::TrueToFalse state
    siAuthorization->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorizing()),siAuthorization->state(DapUiVpnStateIndicator::FalseToTrue));
    siAuthorization->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorized()),siAuthorization->state(DapUiVpnStateIndicator::True));
    siAuthorization->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorized()),siAuthorization->state(DapUiVpnStateIndicator::False));

    // Authorization ::True state
    siAuthorization->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorizing()),siAuthorization->state(DapUiVpnStateIndicator::FalseToTrue));
    siAuthorization->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorizing()),siAuthorization->state(DapUiVpnStateIndicator::TrueToFalse));
    siAuthorization->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorizeError()),siAuthorization->state(DapUiVpnStateIndicator::False));
    siAuthorization->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorized()),siAuthorization->state(DapUiVpnStateIndicator::False));

    //----Stream Opened-----

    // Stream ::False state
    siStream->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpening()),siStream->state(DapUiVpnStateIndicator::FalseToTrue));
    siStream->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpened()),siStream->state(DapUiVpnStateIndicator::True));
    siStream->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosing()),siStream->state(DapUiVpnStateIndicator::TrueToFalse));

    // Stream ::FalseToTrue state
    siStream->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamError()),siStream->state(DapUiVpnStateIndicator::False));
    siStream->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpened()),siStream->state(DapUiVpnStateIndicator::True));
    siStream->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosed()),siStream->state(DapUiVpnStateIndicator::False));
    siStream->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosing()),siStream->state(DapUiVpnStateIndicator::TrueToFalse));

    // Stream ::TrueToFalse state
    siStream->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpening()),siStream->state(DapUiVpnStateIndicator::FalseToTrue));
    siStream->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpened()),siStream->state(DapUiVpnStateIndicator::True));
    siStream->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosed()),siStream->state(DapUiVpnStateIndicator::False));

    // Stream ::True state
    siStream->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpening()),siStream->state(DapUiVpnStateIndicator::FalseToTrue));
    siStream->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosing()),siStream->state(DapUiVpnStateIndicator::TrueToFalse));
    siStream->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamError()),siStream->state(DapUiVpnStateIndicator::False));
    siStream->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosed()),siStream->state(DapUiVpnStateIndicator::False));

    //----NetConf Received-----

    // NetConf ::False state
    siNetConf->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigRequested()),siNetConf->state(DapUiVpnStateIndicator::FalseToTrue));
    siNetConf->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigTrue()),siNetConf->state(DapUiVpnStateIndicator::True));

    // NetConf ::FalseToTrue state
    siNetConf->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigError()),siNetConf->state(DapUiVpnStateIndicator::False));
    siNetConf->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigTrue()),siNetConf->state(DapUiVpnStateIndicator::True));
    siNetConf->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigFalse()),siNetConf->state(DapUiVpnStateIndicator::False));

    // NetConf ::TrueToFalse state
    siNetConf->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigTrue()),siNetConf->state(DapUiVpnStateIndicator::True));
    siNetConf->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigFalse()),siNetConf->state(DapUiVpnStateIndicator::False));

    // NetConf ::True state
    siNetConf->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpening()),siNetConf->state(DapUiVpnStateIndicator::FalseToTrue));

    siNetConf->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigError()),siNetConf->state(DapUiVpnStateIndicator::False));
    siNetConf->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigFalse()),siNetConf->state(DapUiVpnStateIndicator::False));

    //----Tunnel -----

    // Tunnel ::False state
    siTunnel->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreating()),siTunnel->state(DapUiVpnStateIndicator::FalseToTrue));
    siTunnel->state(DapUiVpnStateIndicator::False)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreated()),siTunnel->state(DapUiVpnStateIndicator::True));

    // Tunnel ::FalseToTrue state
    siTunnel->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelError()),siTunnel->state(DapUiVpnStateIndicator::False));
    siTunnel->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreated()),siTunnel->state(DapUiVpnStateIndicator::True));
    siTunnel->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelDestroyed()),siTunnel->state(DapUiVpnStateIndicator::False));
    siTunnel->state(DapUiVpnStateIndicator::FalseToTrue)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelDestroying()),siTunnel->state(DapUiVpnStateIndicator::TrueToFalse));

    // Tunnel ::TrueToFalse state
    siTunnel->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreated()),siTunnel->state(DapUiVpnStateIndicator::True));
    siTunnel->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelDestroyed()),siTunnel->state(DapUiVpnStateIndicator::False));
    siTunnel->state(DapUiVpnStateIndicator::TrueToFalse)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreating()),siTunnel->state(DapUiVpnStateIndicator::FalseToTrue));

    // Tunnel ::True state
    siTunnel->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreating()),siTunnel->state(DapUiVpnStateIndicator::FalseToTrue));

    siTunnel->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelError()),siTunnel->state(DapUiVpnStateIndicator::False));
    siTunnel->state(DapUiVpnStateIndicator::True)
            ->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelDestroyed()),siTunnel->state(DapUiVpnStateIndicator::False));

}


MainWindow::MainWindow(QWidget *parent) :
    DapUiMainWindow(parent)
{

    qDebug() << "[MW] Initializing main window";
    // QWidget::setLayout: Attempting to set QLayout "" on MainWindow "", which already has a layout
    // Layout уже есть
    // https://stackoverflow.com/questions/10519006/qwidgetsetlayout-attempting-to-set-qlayout-on-widget-which-already-has
    //this->setLayout(new QVBoxLayout());

    this->setCentralWidget( pages = new QStackedWidget(this) );
    this->setGeometry(200,100,1280,800);
    pixSpotRed=QPixmap("qrc:/pics/point_red@3x.png");
    pixSpotGreen=QPixmap("qrc:/pics/point_green@3x.png");

    dus = nullptr;
    dusLogin = nullptr;
    dusDashboard = nullptr;

    initTray();

    dlgProgress = NULL;

    // Вынесено во вне, чтобы внесение данных в график не зависило от того, включено ли меню дашбоард.
    connect(&ServiceCtl::me(), &ServiceCtl::sigStatistics, [=](QString r, QString s){
        schedules.addInp(r.toInt());
        schedules.addOut(s.toInt());

        dus->setVars("lbReceived","text",tr("Received: %1Kb").arg(r));
        dus->setVars("lbSent","text",tr("Sent: %1Kb").arg(s));
        if (dusDashboard != nullptr) {
            schedules.draw_chart(
                dusDashboard->getScene(),
                dusDashboard->getSceneWidth() - 3,
                dusDashboard->getSceneHeight() - 3);
            dusDashboard->getScene()->update();
        }
    });

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
        dus = dusLogin = new ScreenLogin(this, pages);
        dus->connectTo<QToolButton>("btRegister", &QToolButton::clicked,[=]{
            QDesktopServices::openUrl(QUrl("https://divevpn.com.ua/registration/"));
        });
        dus->connectTo<QToolButton>("btRestore", &QToolButton::clicked,[=]{
            QDesktopServices::openUrl(QUrl("https://divevpn.com.ua/my-account/lost-password/"));
        });
        dus->setVars("lbStatus","text","");
        dus->setVars("btLogin","enabled",true);
        dus->show();
        connect(dusLogin, &ScreenLogin::reqConnect, [=] (DapServerInfo& dsi,
                QString a_user, QString a_ps) {
            onReqConnect(dsi, a_user, a_ps);
        });

    });
    connect(statesLogin, &QState::exited, [=]{
        qInfo() << "[MainWindow]statesLogin exited";
        delete dus;
    });

    connect(stateLoginCtlConnecting,&QState::entered,[=]{
        qInfo() << "[MainWindow] State LoginCtlConnecting entered";
        dus->setVars("btLogin","enabled",false);
        dus->setVars("cbServer","enabled",false);
        dus->setVars("edPassword","enabled",false);
        dus->setVars("edUsername","enabled",false);
        setStatusText(tr("Backend Reconnect..."));
        setEnabled(false);
        ServiceCtl::me().init();
    });

    connect(stateLoginCtlConnecting, &QState::exited,[=]{
        qInfo() << "[MainWindow] State LoginCtlConnecting exited";
        dus->setVars("btLogin","enabled",true);
        dus->setVars("cbServer","enabled",true);
        dus->setVars("edPassword","enabled",true);
        dus->setVars("edUsername","enabled",true);
        dus->setVars("lbStatus","txt",tr("Connecting..."));
        setStatusText("");
        setEnabled(true);
    });

    connect(stateLoginBegin,&QState::entered, [=]{
        qInfo() << "[MainWindow] State Login Begin";
        dus->setVars("btLogin","enabled",true);
        dus->setVars("cbServer","enabled",true);
        dus->setVars("edPassword","enabled",true);
        dus->setVars("edUsername","enabled",true);
        dus->setVars("btLogin","text",tr("Connect"));
        statesLogin->setInitialState(stateLoginBegin);
    });

    connect(stateLoginConnecting,&QState::entered, [=]{
        qInfo() << "[MainWindow] State Login Connecting";
        dus->setVars("btLogin","text",tr("Stop"));
        dus->setVars("cbServer","enabled",false);
        dus->setVars("edPassword","enabled",false);
        dus->setVars("edUsername","nabled",false);
        dus->setVars("lbStatus","txt",tr("Connecting..."));
    });

    connect(statesDashboard, &QState::entered, [=]{
        qInfo() << "[MainWindow] State statesDashboard entered";
        dus = dusDashboard= new ScreenDashboard(this, pages, m_currentUpstreamName);

        connect(dusDashboard, &ScreenDashboard::currentUpstreamNameChanged, [=](QString name){
           m_currentUpstreamName = name;

        });

        dus->connectTo("btDisconnect",   SIGNAL(clicked(bool)), this, SLOT(onLogout()));
        dus->connectTo("btMessageClose", SIGNAL(clicked(bool)), this, SLOT(onBtMessageCloseClicked()));

        dusDashboard->authorzeIndicator(true);
        dusDashboard->streamIndicator(false);
        dusDashboard->netConfigIndicator(false);
        dusDashboard->tunnelIndicator(false);

        dus->setVars("lbStreamText","text","");
        dus->setVars("lbIpAddrText","text","");
        dus->setVars("lbInterfaceText","text","");
        dus->setVars("lbRouteText","text","");

        setStatusText(tr("Authorized as %1").arg(m_user));
        dus->show();
    });


    // ============== State DashboardCtlConnecting ============
    connect(stateDashboardCtlConnecting,&QState::entered,[=]{
        dus->setVars("btDisconnect","enabled",false);
        dus->setVars("lbReceived","enabled",false);
        dus->setVars("lbSent","enabled",false);
        dus->setVars("lbTimeConnected", "enabled", false);
        setStatusText(tr("Backend Reconnect..."));
    });

    connect(stateDashboardCtlConnecting, &QState::exited,[=]{
        dus->setVars("btLogin","enabled",true);
        dus->setVars("lbReceived","enabled",true);
        dus->setVars("lbSent","enabled",true);
        dus->setVars("cbServer","enabled",true);
        dus->setVars("edPassword","enabled",true);
        dus->setVars("edUsername","enabled",true);
        dus->setVars("lbStatus","txt",tr("Connecting..."));
        setStatusText("");
    });

    connect(statesDashboard, &QState::exited, [=]{
        qInfo() << "[MainWindow] States Dashboard exited";
        dusDashboard = nullptr;
        delete dus;
    });


    connect(stateDashboardConnecting,&QState::entered,[=]{
        qInfo() << "[MainWindow] State Dashboard Connecting";
        dus->setVars("btDisconnect","enabled",false);
        setStatusText("Connecting...");
    });

    connect(stateDashboardConnected,&QState::entered,[=]{
        qInfo() << "[MainWindow] State Dashboard Connected";
        dus->setVars("lbReceived","enabled",true);
        dus->setVars("lbSent","enabled",true);
        dus->setVars("lbReceived","text", "Recieved: 0");
        dus->setVars("lbSent","text", "Sent: 0");

        dus->setVars("btDisconnect","checkable",false);
        dus->setVars("btDisconnect","enabled",true);

        setStatusText("Connected");
    });


    // State Dashboard Disconnecting
    connect(stateDashboardDisconnecting, &QState::entered,[=]{
        qInfo() << "[MainWindow] State DashboardDisconnecting";
        dus->setVars("btDisconnect","checkable",true);
        dus->setVars("btDisconnect","checked",false);
        dus->setVars("btDisconnect","enabled",false);
        dus->setVars("lbStatus","text","Authorized, initializing the tunnel...");
        dus->setVars("lbAuthorizedText","text",tr("User Authorized"));
        dus->setVars("cbAuthorized","checked",true);
        setStatusText("Disconnecting...");
        ServiceCtl::me().sendCmd("disconnect");
    });

    // * ---> *CtlConnecting
    statesLogin->addTransition(&ServiceCtl::me(),SIGNAL(ctlDisconnected()),stateLoginCtlConnecting);
    statesDashboard->addTransition(&ServiceCtl::me(),SIGNAL(ctlDisconnected()),stateDashboardCtlConnecting);

    // *CtlConnecting ---> *
    stateLoginCtlConnecting->addTransition(&ServiceCtl::me(),SIGNAL(ctlConnected()),stateLoginBegin);
    stateDashboardCtlConnecting->addTransition(&ServiceCtl::me(), SIGNAL(ctlConnected()), stateDashboardConnected);

    // LoginBegin ---> LoginConnecting
    stateLoginBegin->addTransition(this,SIGNAL(sigBtConnect()),stateLoginConnecting );

    // LoginBegin ---> Dashboard
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorized()),statesDashboard );
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpened()),statesDashboard );
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigTrue()),statesDashboard );
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreated()),statesDashboard );

    // LoginConnecting ---> LoginBegin
    stateLoginConnecting->addTransition(this,SIGNAL(sigBtConnect()),stateLoginBegin);
    stateLoginConnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorizeError()),stateLoginBegin);

    // LoginConnecting ---> Dashboard
    stateLoginConnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorized()),statesDashboard );
    stateLoginConnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpened()),statesDashboard );
    stateLoginConnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigTrue()),statesDashboard );
    stateLoginConnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreated()),statesDashboard );

    // Dashboard ---> DashboardDisconnecting
   // statesDashboard->addTransition(this,SIGNAL(sigBtDisconnect()),stateDashboardDisconnecting);

    // DashboardDisconnectin ---> Begin
    stateDashboardDisconnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorized()),stateLoginBegin);

    // Login Ctl Connecting ---> Login Begin
    stateLoginCtlConnecting->addTransition(&ServiceCtl::me(),SIGNAL(ctlConnected()),stateLoginBegin);

    // Login Begin ---> Login Connecting
    stateLoginBegin->addTransition(this,SIGNAL(sigBtConnect()),stateLoginConnecting );

    // Begin ---> Dashboard
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateAuthorized()),stateDashboardConnected );
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamOpened()),stateDashboardConnected );
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigTrue()),stateDashboardConnected );
    stateLoginBegin->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelCreated()),stateDashboardConnected );

    // DashboardConnecting --> DashboardConnected
    stateDashboardConnecting->addTransition(&ServiceCtl::me() ,SIGNAL(sigStateTunnelCreated()),stateDashboardConnected );

    // Dashboard ---> DashboardDisconnecting
    stateDashboardConnected->addTransition(this,SIGNAL(sigBtDisconnect()),stateDashboardDisconnecting);
    stateDashboardConnected->addTransition(&ServiceCtl::me(),SIGNAL(sigStateStreamClosed()) ,stateDashboardConnecting);
    stateDashboardConnected->addTransition(&ServiceCtl::me(),SIGNAL(sigStateTunnelDestroyed()) ,stateDashboardConnecting);
    stateDashboardConnected->addTransition(&ServiceCtl::me(),SIGNAL(sigStateNetConfigFalse()) ,stateDashboardConnecting);
    stateDashboardConnected->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorized()) , stateDashboardConnecting);


    // DashboardDisconnecting ---> Begin
    stateDashboardDisconnecting->addTransition(&ServiceCtl::me(),SIGNAL(sigStateUnauthorized()),stateLoginBegin);

    initIndicators();
    initIndicatorsTransitions();

    sm.start();
}
