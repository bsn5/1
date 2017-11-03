#ifndef SCREENDASHBOARD_H
#define SCREENDASHBOARD_H

#include <QGraphicsScene>
#include "ScreenVpnAbstract.h"


class ScreenDashboard : public ScreenVpnAbstract
{
    Q_OBJECT

    QGraphicsScene *m_scene;
    int m_sceneWidth;
    int m_sceneHeight;

protected:
    void initUi(QWidget * a_w,ScreenRotation a_rotation);
public:
    void authorzeIndicator(bool on);
    void streamIndicator(bool on);
    void netConfigIndicator(bool on);
    void tunnelIndicator(bool on);

    QGraphicsScene* getScene() {return m_scene;}
    int getSceneWidth() {return m_sceneWidth;}
    int getSceneHeight() {return m_sceneHeight;}

    ScreenDashboard(QObject * a_parent, QStackedWidget * a_sw, QString upstreamName);
signals:
    void reconnectSignal(QString adr, QString ip);
    void currentUpstreamNameChanged(QString name);
};

#endif // DAPUIDASHBOARD_H
