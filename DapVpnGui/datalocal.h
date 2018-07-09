#ifndef DATALOCAL_H
#define DATALOCAL_H

#include <QList>
#include <QString>
#include <QObject>
#include <QColor>
#include <QMap>
#include <QPair>
#include <QSettings>


#define SERVER_LIST_FILE "DiveVPNServers.xml"

class DapServerInfo
{
public:
    DapServerInfo(){}
    DapServerInfo(const DapServerInfo&cp) {
        address     = cp.address;
        port        = cp.port;
        name        = cp.name;
        location    = cp.location;
        ip          = cp.ip;
    }

    QString address;
    QString port;
    QString name;
    QString location;
    QString ip;
};

class DataLocal : public QObject
{
    Q_OBJECT
    const QString ServerListName;
public:
    static DataLocal * me(){ return _me?_me: _me = new DataLocal();}

    void setSetting(const QString & a_name,const QVariant &a_value){
        m_settings->setValue(a_name,a_value);
    }
    QVariant setting(const QString& a_name){ return m_settings->value(a_name);}

    void addServer(const QString& a_location, const QString& a_name, const QString & a_ip, const QString& a_addrLine );

    QList<DapServerInfo>& servers(){return m_servers;}
    const DapServerInfo& serverTheBest(){ return m_servers.at(0) ;  }
    const QString& locationToIcon(const QString & a_location);
    const QString& sizeSuffix();
    const QString& title(){ return m_title;}
    int loginWidth(){return m_loginWidth;}
    int loginHeight(){return m_loginHeight;}
    int mainWidth(){return m_mainWidth;}
    int mainHeight(){return m_mainHeight;}
    const QColor& sliderColor(){return m_sliderColor;}
private:
    static DataLocal *_me;
    DataLocal();

    QList<DapServerInfo> m_servers;

    int m_votingBorderWidth;

    int m_loginWidth;
    int m_loginHeight;
    int m_mainWidth;
    int m_mainHeight;
    bool m_votingEnabled;
    bool m_ratingEnabled;
    QColor m_votingHighlightColor;
    QColor m_sliderColor;
    QString m_settingsFileName;
    QSettings * m_settings;

    QString m_title;
    void parseXML(const QString& a_fname);
};

#endif // DATALOCAL_H
