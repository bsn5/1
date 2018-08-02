#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLinkedList>
#include <QMap>
#include <QGraphicsBlurEffect>
#include <QStackedWidget>
#include <QStateMachine>
#include <QState>
#include <QHistoryState>
#include <QFinalState>

#include "schedules.h"
#include "datalocal.h"
#include "usrmsg.h"
#include "ScreenLogin.h"
#include "ScreenDashboard.h"
#include "DapUiMainWindow.h"
#include "DapUiVpnStateIndicator.h"

class QSystemTrayIcon;
class QCloseEvent;
#ifdef DAP_SERVICE_CONNECT_TCP
class QTcpSocket;
#else
class QLocalSocket;
#endif
class QProgressDialog;
class QMenu;
class QAction;

enum MsgType {Info, Alert, Error};

class MainWindow : public DapUiMainWindow
{
    Q_OBJECT
private:
    QStackedWidget * pages;

    DapUiScreen * dus;
    ScreenDashboard * dusDashboard;
    ScreenLogin * dusLogin;

    QPixmap pixSpotRed;
    QPixmap pixSpotGreen;

    Schedules schedules;

    void initIndicators();
    void initIndicatorsTransitions();

    bool userMsgQueueIsEmpty() {return dashboard_user_msg == "";}


    QString dashboard_user_msg = "";

    void sendUpstreamsToServer();
    void updateUpstreams();

    void initTray();


    UsrMsg     user_msg;

    QSystemTrayIcon * trayIcon;
    QMenu * trayMenu;
    QString m_user, m_password;
    QString m_addrGW;
    int m_socketUpstream;
    QString m_addrAssigned;
    QColor m_statusColor;

    QStateMachine sm;
        QState * statesUser;
            QHistoryState * stateUserHistoryState;
            QState * statesLogin;
                QHistoryState * stateLoginHistoryState;
                QState * stateLoginCtlConnecting;
                QState * stateLoginBegin;
                QState * stateLoginConnecting;
            QState * statesDashboard;
                QHistoryState * stateDashboardHistoryState;
                QState * stateDashboardCtlConnecting;
                QState * stateDashboardConnected;
                QState * stateDashboardConnecting;
                QState * stateDashboardDisconnecting;
            /*
            QState * stateAbout;
            QState * stateSettings;
            */
            QState * stateExiting;
            QFinalState * stateExit;

        QState * statesIndicators;

            DapUiVpnStateIndicator *siAuthorization;
            DapUiVpnStateIndicator *siStream;
            DapUiVpnStateIndicator *siNetConf;
            DapUiVpnStateIndicator *siTunnel;

    QList<DapUiVpnStateIndicator *> siAll;
    QString m_currentUpstreamName;

    // блокируем всплытие контекстного меню
    void contextMenuEvent(QContextMenuEvent *event) override { Q_UNUSED(event); }

    int m_calcBack;

    QProgressDialog * dlgProgress;

#ifdef DAP_SERVICE_CONNECT_TCP
    QTcpSocket * sockCtl;
#else
    QLocalSocket * sockCtl;
#endif
    QAction * actionDisconnect;
    QAction * actionSettings;
    QAction * actionExit;

    int checkLeftSeconds;
signals:
    void sigBtConnect();
    void sigBtDisconnect();

private slots:

    void onBtMessageCloseClicked();

    void onReqConnect(const DapServerInfo& dsi, QString a_user, QString a_ps);

    void raiseErrorDialog(const QString &text);
    void raiseNotifyDialog(const QString &text);
    void raiseProgressDialog(const QString &text);
    void updateUsrMsg();
    void hideProgressDialog();
    void updateProgress(int value);

    void onExit();

    void setStatusText(const QString & a_txt);
    void addMessage(const QString& a_msg, MsgType a_type){ Q_UNUSED(a_msg); Q_UNUSED(a_type); }
    void onLogout();
public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
};

#endif // MAINWINDOW_H
