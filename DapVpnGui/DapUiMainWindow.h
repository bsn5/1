#ifndef DAPUIMAINWINDOW_H
#define DAPUIMAINWINDOW_H

#include <QMainWindow>
#include <QScreen>
#include <QVariant>
#include <QtDebug>
class QStackedWidget;
#include "DapUiScreen.h"

class DapUiMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DapUiMainWindow(QWidget *parent = 0);
    virtual ~DapUiMainWindow();
    QStackedWidget * sw() { return m_sw; }
    DapUiScreen * screen() { return m_screen; }
    QString screenName(){ return QString::fromLatin1(screen()?screen()->metaObject()->className():"nullptr" ); }
    void newScreen(DapUiScreen * a_screen);

    void setUiProp(const QString& a_objName, const QString& a_varName, const QVariant& a_varValue){
        if(screen())
            screen()->setVars(a_objName, a_varName, a_varValue);
    }

    DapUiScreen::ScreenRotation rotation() { return m_rotation; }
    static DapUiMainWindow *getInstance() { return m_instance; }

    template <class T1, typename Func1, class T2, typename Func2>
    inline void
    connectScreen(Func1 a_signal, T2* a_receiver, Func2 a_slot  ){
        if(screen())
            connect(qobject_cast<T1*>(screen()),a_signal, a_receiver, a_slot );
        else
            qWarning() << "[DapUiMainWindow] no current screen to connecto to";
    }

    template <class T1, typename Func1, typename Func2>
    inline void
    connectScreen(Func1 a_signal, Func2 a_lambda  ){
        if(screen())
            connect(qobject_cast<T1*>(screen()),a_signal, a_lambda );
        else
            qWarning() << "[DapUiMainWindow] no current screen to connecto to";
    }

signals:
public slots:

protected:
    void onScreenOrientationChanged(Qt::ScreenOrientation a_orientation);

private:
    QStackedWidget * m_sw;
    DapUiScreen * m_screen = nullptr;
    DapUiScreen::ScreenRotation m_rotation;

    static DapUiMainWindow * m_instance;
private slots:
};

#endif // DAPUIMAINWINDOW_H
