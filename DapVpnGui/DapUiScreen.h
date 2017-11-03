#ifndef DAPUISCREEN_H
#define DAPUISCREEN_H

#include <QObject>
#include <QWidget>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QStackedWidget>

//#include <qobject_impl.h>

class QStackedWidget;

class DapUiScreen : public QObject
{
    Q_OBJECT
public:
    enum ScreenRotation{Hor,Ver,HorInv,VerInv};
    enum ScreenSize{Small=0,Medium=10,Big=20};
    Q_ENUM(ScreenRotation);
    Q_ENUM(ScreenSize);
private:
    QStackedWidget* m_sw;
    QSet<ScreenRotation> m_rotations;
    ScreenRotation m_rotation;
protected:

    const QSet<ScreenRotation>& rotations(){ return  m_rotations;}
    QWidget * page(ScreenRotation a_rotation){ return m_page[a_rotation];}
    QMap<ScreenRotation,QWidget*> m_page;


    template<class T,class TSmall, class TBig>
    void initUiForm(QWidget * a_w)
    {
        switch(getScreenSize()){
            case Big: {
                static TBig * l_ui = nullptr;
                if( l_ui)
                    delete l_ui;
                l_ui = new TBig;
                l_ui->setupUi(a_w);
            } break;
            case Small: {
                static TSmall * l_ui = nullptr;
                if( l_ui)
                    delete l_ui;
                l_ui=new TSmall;
                l_ui->setupUi(a_w);
            } break;
            default: {
                static T * l_ui = nullptr;
                if( l_ui) delete l_ui;
                l_ui = new T;
                l_ui->setupUi(a_w);
            }
        }
    }

    virtual void initUi(QWidget * a_w,ScreenRotation a_rotation)=0;

    QStackedWidget * sw(){ return m_sw;};

public:
    DapUiScreen(QObject *parent, QStackedWidget * a_sw);
    virtual ~DapUiScreen();

#ifdef DAP_PLATFORM_DESKTOP
    template<class T,class TSmall, class TBig>
#else
    template<class THor,class THorSmall, class THorBig, class TVer,class TVerSmall, class TVerBig>
#endif
    inline void create(){
            for (auto rot: rotations() ){
                QWidget *p;
                m_page.insert(rot,p=new QWidget(sw()) );
                sw()->addWidget(p );
#ifdef DAP_PLATFORM_DESKTOP
                initUiForm<T,TSmall,TBig>(p);
#else
                switch(rot){
                    case Ver: initUiForm<TVer,TVerSmall,TVerBig>(p); break;
                    default: initUiForm<THor,THorSmall,THorBig>(p); break;
                }
#endif
                initUi(p,rot);
            }
            rotate(Hor);
    }
#ifdef  DAP_PLATFORM_MOBILE
    template<class THor,class THorSmall, class THorBig, class TVer,class TVerSmall, class TVerBig,
     class THorInv,class THorInvSmall, class THorInvBig, class TVerInv,class TVerInvSmall, class TVerInvBig>
    inline void createAll(){
            for (auto rot: rotations() ){
                QWidget *p;
                sw()->addWidget( m_page.insert(rot,p=new QWidget(sw()) ) );
                switch(rot){
                    case Ver: initUiForm<TVer,TVerSmall,TVerBig>(p); break;
                    case VerInv: initUiForm<TVerInv,TVerInvSmall,TVerInvBig>(p); break;
                    case HorInv: initUiForm<THorInv,THorInvSmall,THorInvBig>(p); break;
                    default: initUiForm<THor,THorSmall,THorBig>(p); break;
                }
                initUi(p,rot);
            }
            sw()->setCurrentIndex(0);
    }
#endif
    virtual void show();
    virtual void update();
    void rotate(ScreenRotation a_rot);
    ScreenRotation rotation(){ return m_rotation; };
    static ScreenSize getScreenSize();
    QWidget * currentPage(){ return sw()->currentWidget(); }
    void setVars(const QString& a_objName, const QString& a_varName, const QVariant& a_varValue)
    {
        foreach (auto rotation, rotations()) {
            QWidget *w = getWidget(a_objName,rotation);
            if(w)
                w->setProperty(a_varName.toLatin1().constData(),a_varValue);
        }
    }

    void invokeMethods(const QString& a_objName, const QString& a_methodName
                       , QGenericArgument a_val0=QGenericArgument(nullptr)
                       , QGenericArgument a_val1=QGenericArgument()
                       , QGenericArgument a_val2=QGenericArgument()
                       , QGenericArgument a_val3=QGenericArgument()
                       , QGenericArgument a_val4=QGenericArgument()
                       , QGenericArgument a_val5=QGenericArgument()
                       , QGenericArgument a_val6=QGenericArgument()
                       , QGenericArgument a_val7=QGenericArgument()
                       , QGenericArgument a_val8=QGenericArgument()
                       , QGenericArgument a_val9=QGenericArgument()
            )
    {
        foreach (auto rotation, rotations()) {
            QWidget *w = getWidget(a_objName,rotation);
            QGenericReturnArgument ret;
            if(w){
               QMetaObject::invokeMethod(w, a_methodName.toLatin1().constData(),ret,a_val0,
                                         a_val1,a_val2,a_val3, a_val4, a_val5, a_val6, a_val7, a_val8,a_val9);
            }
        }
    }

    template <class T, typename Func1,typename Func2>
    inline void
    connectTo(const QString& a_objName, Func1 a_signal, Func2 a_func  ){
        foreach (auto rotation, rotations()) {
            T * w=page(rotation)->findChild<T*>(a_objName);
            if(w)
                connect(qobject_cast<T*>(w),a_signal,a_func);
        }
    }
    void connectTo(const QString& a_objName,const char *a_signal, const QObject *a_recv, const char *a_slot );

//    void connectTo(const QString& a_objName, PointerToMemberFunction a_signal,const QObject *a_recv,PointerToMemberFunction a_slot );
//    void connectTo(const QString& a_objName,const char *a_signal, const QObject *a_recv, const char *a_slot );

    QWidget* getWidget(const QString& a_objName,ScreenRotation a_rot) {  return page(a_rot)->findChild<QWidget*>(a_objName); }
    template <class T>
    T* getWidgetCustom(const QString& a_objName,ScreenRotation a_rot) {  return page(a_rot)->findChild<T*>(a_objName); }
signals:
public slots:

};

#endif // DAPUISCREEN_H
