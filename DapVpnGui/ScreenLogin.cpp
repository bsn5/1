#include <QtDebug>
#include <QSettings>

#ifdef DAP_PLATFORM_DESKTOP
#include "ui_login_desktop.h"
#include "ui_login_desktop_big.h"
#include "ui_login_desktop_small.h"
#else
#include "ui_login_mobile_hor.h"
#include "ui_login_mobile_hor_big.h"
#include "ui_login_mobile_hor_small.h"
#include "ui_login_mobile_ver.h"
#include "ui_login_mobile_ver_big.h"
#include "ui_login_mobile_ver_small.h"
#endif

#include "datalocal.h"
#include "ScreenLogin.h"

/**
 * @brief ScreenLogin::ScreenLogin
 * @param a_parent
 * @param a_sw
 */
ScreenLogin::ScreenLogin(QObject * a_parent, QStackedWidget * a_sw )
    :ScreenVpnAbstract(a_parent,a_sw)
{
    // Инициализация формы ( в зависимости от размера экрана)
#ifdef DAP_PLATFORM_MOBILE
    create<Ui::LoginHor,Ui::LoginHorSmall,Ui::LoginHorBig,
            Ui::LoginVer,Ui::LoginVerSmall,Ui::LoginVerBig>();
/*
    QWidget * cbUpstreamHor=getWidget("cbUpstream",Hor);
    QWidget * cbUpstreamVer=getWidget("cbUpstream",Ver);
    QLineEdit * edPasswordHor=getWidgetCustom<QLineEdit>("edPassword",Hor);
    QLineEdit * edPasswordVer=getWidgetCustom<QLineEdit>("edPassword",Ver);
    Q_ASSERT(edPasswordHor);
    Q_ASSERT(edPasswordVer);
    Q_ASSERT(cbUpstreamHor);
    Q_ASSERT(cbUpstreamVer);
    // перекрёсное связывание выбора сервера
    // сервер выбранный в одном интерфейсе, окажется выбран и в другом
    connect(cbUpstreamHor, SIGNAL(currentIndexChanged(int))
            ,cbUpstreamVer, SLOT(setCurrentIndex(int)));
    connect(cbUpstreamVer, SIGNAL(currentIndexChanged(int))
            ,cbUpstreamHor, SLOT(setCurrentIndex(int)));

    // перекрёсное связывание паролей
    // пароль введённый в одном окне, появится и в другом
    connect( edPasswordHor,&QLineEdit::textChanged
            ,edPasswordVer,&QLineEdit::setText );
    connect(edPasswordVer ,&QLineEdit::textChanged
            ,edPasswordHor,&QLineEdit::setText );
*/
#else
    create<Ui::Login,Ui::LoginSmall,Ui::LoginBig>();
#endif

}


/**
 * @brief DapUiScreenLogin::initUiCommonAfter
 * @param a_w
 */
void ScreenLogin::initUi(QWidget * a_w,ScreenRotation a_rotation)
{
    Q_UNUSED(a_rotation);
    QLabel * lbMessage=a_w->findChild<QLabel*>("lbMessage");

    QLineEdit * edPassword=a_w->findChild<QLineEdit*>("edPassword");
    QLineEdit * edUsername=a_w->findChild<QLineEdit*>("edUsername");
    QPushButton * btLogin=a_w->findChild<QPushButton*>("btLogin");
    QComboBox *cbUpstream=a_w->findChild<QComboBox*>("cbUpstream");

    Q_ASSERT(lbMessage);
    Q_ASSERT(edPassword);
    Q_ASSERT(edUsername);
    Q_ASSERT(btLogin);

    QToolButton *btHelp = a_w->findChild<QToolButton*>("btHepl");
    QToolButton *btSettings = a_w->findChild<QToolButton*>("btSettings");

    connect(btSettings, &QToolButton::clicked, [=]{emit reqSetting();});
    connect(btHelp, &QToolButton::clicked, [=]{emit reqAbout();});
    connect(edPassword, &QLineEdit::returnPressed, btLogin, &QPushButton::click);
    connect(btLogin, &QPushButton::clicked, [=]{emit reqConnect(currentUpstreamAddr(),upstreamIp(currentUpstreamAddr()), edUsername->text(),edPassword->text()); });
    connect(cbUpstream,&QComboBox::currentTextChanged ,[=]{
        m_currentUpstreamName = cbUpstream->currentText();
        m_currentUpstreamAddr = cbUpstream->currentData().toString();
        emit currentUpstreamNameChanged(m_currentUpstreamName);
    });

    QSettings settings;
    edUsername->setText(settings.value("username").toString());
    edPassword->setText(settings.value("password").toString());

    fillUpstreams(a_w);
}



