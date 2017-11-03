#include <QtDebug>
#include <QStackedWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QLayout>
#include "DapUiMainWindow.h"

/**
 * @brief DapUiMainWindow::DapUiMainWindow
 * @param parent
 */
DapUiMainWindow * DapUiMainWindow::m_instance=nullptr;

DapUiMainWindow::DapUiMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(new QVBoxLayout() );
    m_sw = new QStackedWidget(centralWidget());
    centralWidget()->layout()->addWidget(m_sw);

    m_instance = this;
    m_screen = nullptr;
    QGuiApplication::primaryScreen()->setOrientationUpdateMask(
                Qt::LandscapeOrientation | Qt::PortraitOrientation |
                Qt::InvertedLandscapeOrientation | Qt::InvertedLandscapeOrientation);
    connect(QGuiApplication::primaryScreen(),&QScreen::orientationChanged
            ,this, &DapUiMainWindow::onScreenOrientationChanged);

    onScreenOrientationChanged(QGuiApplication::primaryScreen()->orientation());

}

/**
 * @brief DapUiMainWindow::~DapUiMainWindow
 */
DapUiMainWindow::~DapUiMainWindow()
{
   if(m_screen)
       delete m_screen;
}


/**
 * @brief DapUiMainWindow::onScreenOrientationChanged
 * @param a_orientation
 */
void DapUiMainWindow::onScreenOrientationChanged(Qt::ScreenOrientation a_orientation)
{

    switch (a_orientation){
        case Qt::PortraitOrientation:
        case Qt::InvertedPortraitOrientation:{
            m_rotation=DapUiScreen::Ver;
            if(screen())
                screen()->update();
        }break;
        case Qt::LandscapeOrientation:
        case Qt::InvertedLandscapeOrientation:
        default:{
            m_rotation=DapUiScreen::Hor;
            if(screen())
                screen()->update();
        }
    }
}

/**
 * @brief DapUiMainWindow::newScreen
 * @param a_screen
 */
void DapUiMainWindow::newScreen(DapUiScreen * a_screen)
{
    if(m_screen)
        delete m_screen;
    m_screen=a_screen;
    m_screen->update();
}
