#ifndef DATALOCAL_H
#define DATALOCAL_H

#include <QList>
#include <QString>
#include <QObject>
#include <QColor>
#include <QMap>
#include <QPair>
#include <QSettings>
#include "DapServerInfo.h"

#define SERVER_LIST_FILE "DiveVPNServers.xml"



class DataLocal : public QObject
{
    Q_OBJECT
    const QString ServerListName;
public:
    using picturesMap = QMap<DapServerLocation, QString>;
    static DataLocal * me(){ return _me?_me: _me = new DataLocal();}

    void setSetting(const QString & a_name,const QVariant &a_value){
        m_settings->setValue(a_name,a_value);
    }
    QVariant setting(const QString& a_name){ return m_settings->value(a_name);}

    void addServer(DapServerLocation location, const QString& name,
                   const QString & address, quint16 port, QString ip = "");
    void addServer(const DapServerInfo& dsi);

    QList<DapServerInfo>& servers(){return m_servers;}
    const DapServerInfo& serverTheBest(){ return m_servers.at(0) ;  }
    QString locationToIconPath(DapServerLocation loc);
    const QString& sizeSuffix();
    const QString& title(){ return m_title;}
    int loginWidth(){return m_loginWidth;}
    int loginHeight(){return m_loginHeight;}
    int mainWidth(){return m_mainWidth;}
    int mainHeight(){return m_mainHeight;}
    const QColor& sliderColor(){return m_sliderColor;}
private:
    static DataLocal *_me;
    static QMap<DapServerLocation, QString> m_pictruePath;

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
