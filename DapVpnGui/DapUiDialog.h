#ifndef DAPUIDIALOG_H
#define DAPUIDIALOG_H
#include <QtDebug>
#include <QAbstractButton>
#include "DapUiScreen.h"


class DapUiDialog : public DapUiScreen
{
Q_OBJECT
protected:
    QWidget * w;
    QWidget * wPrev;
public:
    DapUiDialog(QObject * parent, QStackedWidget * a_sw): DapUiScreen(parent,a_sw){}

    template<class TUi,typename TFuncClosed>
    void createDialog(TFuncClosed a_funcClosed)
    {
        static TUi ui= nullptr;
        if ( ui == nullptr)
                delete ui;
        ui = new TUi;

        sw()->addWidget(w=new QWidget(sw()));
        m_page .insert(Hor,w);
        ui->setupUi(w);
        connect(ui->btClose,&QAbstractButton::clicked, a_funcClosed );
        connect(ui->btClose,&QAbstractButton::clicked, [=] { sw()->setCurrentWidget(wPrev); } );
    }

    void initUi(QWidget * a_w,ScreenRotation a_rotation){
        Q_UNUSED(a_w); Q_UNUSED(a_rotation);
        qWarning() << "[DapUiDialog] initUi() shouldn't be called";
    }


    void show(){
        wPrev= sw()->currentWidget();
        sw()->setCurrentWidget(w);
    }
};

#endif // DAPUIDIALOG_H
