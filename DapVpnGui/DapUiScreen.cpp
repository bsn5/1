#include <QtDebug>
#include <QStackedWidget>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>
#include "DapUiScreen.h"
#include "DapUiMainWindow.h"

/**
 * @brief DapUiScreen::DapUiScreen
 * @param parent
 * @param a_sw
 */
DapUiScreen::DapUiScreen(QObject *parent, QStackedWidget * a_sw)
    : QObject(parent),m_sw(a_sw)
{
    Q_ASSERT(m_sw);

// Set up default rotations, childs could change that by their own
    m_rotations.insert(Hor);
#ifdef DAP_UI_TYPE_MOBILE
    m_rotations.insert(Ver);
#endif

}

DapUiScreen::~DapUiScreen()
{
//    for(auto l_page: m_page)
//        delete l_page;
}

DapUiScreen::ScreenSize DapUiScreen::getScreenSize()
{
    ScreenSize ret=Medium;
    QRect avGem=QGuiApplication::primaryScreen()->availableGeometry();
    int h=avGem.height();
    int w=avGem.width();
    if(h>w){
        w=avGem.height();
        h=avGem.width();
    }
#ifdef DAP_PLATFORM_DESKTOP
    if(h<800)
        ret=Small;
    if(h>1500)
        ret=Big;
#else
    if(h<700)
        ret=Small;
    if(h>1400)
        ret=Big;
#endif
    return ret;
}

/**
 * @brief DapUiScreen::show
 */
void DapUiScreen::show()
{
   rotate(rotation());
}

/**
 * @brief DapUiScreenAbstract::rotate
 * @param a_rot
 */
void DapUiScreen::rotate(ScreenRotation a_rot)
{
    m_rotation=a_rot;
    if(rotations().contains(a_rot))
        sw()->setCurrentWidget(page(a_rot));
}



/**
 * @brief DapUiScreenAbstract::connectTo
 * @param a_objName
 * @param a_signal
 * @param a_recv
 * @param a_slot
 */
void DapUiScreen::connectTo(const QString& a_objName,const char *a_signal, const QObject *a_recv, const char *a_slot )
{
    foreach (auto rotation, rotations()) {
        QWidget * w = getWidget(a_objName,rotation);
        if(w)
            connect(w,a_signal,a_recv,a_slot);
    }
}


void DapUiScreen::update()
{
    if(rotations().contains( DapUiMainWindow::getInstance()->rotation())){
        QWidget * w = page(DapUiMainWindow::getInstance()->rotation());
        if(w != sw()->currentWidget()){
            sw()->setCurrentWidget(w);
        }
    }
}
