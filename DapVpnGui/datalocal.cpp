#include <QStandardPaths>

#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QSettings>
#include <QCoreApplication>

#include "datalocal.h"

DataLocal::picturesMap DataLocal::m_pictruePath = {
    {DapServerLocation::ENGLAND, ":/country/Flag_uk.png"},
    {DapServerLocation::FRANCE, ":/country/Flag_fr.png"},
    {DapServerLocation::GERMANY, ":/country/Flag_de.png"},
    {DapServerLocation::USA, ":/country/Flag_us.png"},
    {DapServerLocation::NETHERLANDS, ":/country/Flag_nl.png"},
    {DapServerLocation::RUSSIA, ":/country/Flag_ru.png"},
    {DapServerLocation::UKRAINE, ":/country/Flag_ua.png"},
    {DapServerLocation::USSR, ":/country/ussr.png"},
};

DataLocal *DataLocal::_me = NULL;

DataLocal::DataLocal()
{
    qDebug() << "[DL] Constructor";
    m_title = "DapVPN";

    m_ratingEnabled=true;
    m_loginWidth=400;
    m_loginHeight=300;

    m_mainWidth=1000;
    m_mainHeight=400;

    m_votingBorderWidth=1;
    QCoreApplication::setOrganizationName("veiledtech");
    QCoreApplication::setOrganizationDomain("veiledtech.com");
    QCoreApplication::setApplicationName("DAP_BRAND");
    m_settings = new QSettings();

    parseXML(":/data.xml");

}

void DataLocal::parseXML(const QString& a_fname)
{
    qDebug() << "[DL] Constructor";
    QFile file(a_fname);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "[DL] Can't open data.xml from built in resource";
    }

    QXmlStreamReader *sr = new QXmlStreamReader(&file);
    if(sr->readNextStartElement()){
        if(sr->name() == "data"){
            while ( sr->readNextStartElement() ){
                if( sr->name() == "servers"){
                    while ( sr->readNextStartElement() ){
                        if( sr->name() == "server") {
                            DapServerInfo item;
                            while ( sr->readNextStartElement() ){
                                if(sr->name() == "name"){
                                    item.name=sr->readElementText();
                                } else if (sr->name() == "address") {
                                    item.address=sr->readElementText();
                                } else if( sr->name() == "port") {
                                    item.port=sr->readElementText();
                                } else if(sr->name() == "location") {
                                    item.location = DapServerInfo::stringToLaction(sr->readElementText());
                                } else if (sr->name() == "ip") {
                                    item.ip = sr->readElementText();
                                } else {
                                    qDebug() << "[DL] Inside tag 'server': Unknown tag "<<sr->name();
                                    sr->skipCurrentElement();
                                }
                            }
                            qDebug() << "[DL] Server "<<item.name<<" added";
                            m_servers.push_back(item);
                        }else{
                            qDebug() << "[DL] Inside tag 'servers': unknown tag "<<sr->name();
                            sr->skipCurrentElement();
                        }
                    }
                }if( sr->name() == "interface"){
                    while ( sr->readNextStartElement() ){
                        if (sr->name() == "title"){
                            m_title=sr->readElementText();
                        }
                    }
                }else{
                    qDebug() << "[DL] Inside tag 'data' unknown tag "<<sr->name();
                    //sr->skipCurrentElement();
                }
            }
        }
    }
#ifdef  QT_DEBUG
    addServer(DapServerLocation::FRANCE, "fr-dev", "ap-fr-0.divevpn.demlabs.net", "ap-fr-0.divevpn.demlabs.net:8002");
    addServer(DapServerLocation::NETHERLANDS, "nl-dev", "ap-nl-0.divevpn.demlabs.net", "ap-nl-0.divevpn.demlabs.net:8002");
    addServer(DapServerLocation::NETHERLANDS, "local", "127.0.0.1", "127.0.0.1:8002");
#endif


    delete sr;
}

void DataLocal::addServer(const DapServerInfo& dsi) {
    for(auto& s: servers()) {
        if(s == dsi)
            return;
    }
    m_servers.push_back(dsi);
}

void DataLocal::addServer(DapServerLocation a_location, const QString& a_name, const QString & a_ip, const QString& a_addrLine )
{
    DapServerInfo ss;
    ss.name = a_name ;
    ss.location = a_location;
    ss.address = a_addrLine.split(":").at(0);
    ss.port = a_addrLine.split(":").at(1);
    ss.ip = a_ip;
    addServer(ss);
}

/**
 * @brief DataLocal::locationToIcon
 * @param a_location
 * @return
 */
QString DataLocal::locationToIconPath(DapServerLocation loc)
{
    QString locPath = m_pictruePath.value(loc);
    if (locPath == "") {
        qWarning() << "Not found picture for current location. Return default!";
        return m_pictruePath.value(DapServerLocation::USSR);
    }
    return locPath;
}
