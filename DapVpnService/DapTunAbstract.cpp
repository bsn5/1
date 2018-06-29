#include <QtDebug>
#include <QXmlStreamReader>
#include <QDomElement>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QFile>
#include <unistd.h>

#include "DapTunAbstract.h"
#include "DapSockForwPacket.h"
#include "staticdata.h"

#define STREAM_SF_PACKET_OP_CODE_RAW_SEND 0xbc

/**
 * @brief DapTunAbstract::DapTunAbstract
 */
DapTunAbstract::DapTunAbstract()
    : QObject(nullptr)
{
    m_MTU=0xfffff;
    tunThread = new QThread();
    tunWorker = nullptr;
}

/**
 * @brief DapTunAbstract::isCreated
 */
bool DapTunAbstract::isCreated()
{
    return m_isCreated;
}

/**
 * @brief DapTunAbstract::initWorker
 */
void DapTunAbstract::initWorker()
{
    qDebug() << "[DapTunAbstract::initWorker]";

    connect(tunThread,&QThread::started, tunWorker,&DapTunWorkerAbstract::loop);

    connect(tunWorker,&DapTunWorkerAbstract::loopStarted,this,&DapTunAbstract::created);

    connect(tunWorker,&DapTunWorkerAbstract::packetOut,this,&DapTunAbstract::packetOut);
    connect(tunWorker,&DapTunWorkerAbstract::loopError,this,&DapTunAbstract::error);

    connect(tunWorker,&DapTunWorkerAbstract::loopStopped,this,&DapTunAbstract::onWorkerStopped);
    connect(tunWorker,&DapTunWorkerAbstract::loopStopped,this,&DapTunAbstract::tunDeviceDestroy);

}

/**
 * @brief DapTunAbstract::create
 * @param a_addr
 * @param a_gw
 * @param a_upstreamAddress
 * @param a_upstreamSocket
 * @details Create tunnel with pointed address, gateway and protection of upstream socket and address
 */
void DapTunAbstract::create(const QString &a_addr, const QString &a_gw, const QString & a_upstreamAddress, int a_upstreamSocket)
{
    qInfo() << " address = "<< a_addr<<" gw = "<<a_gw << " upstreamAddress = " <<a_upstreamAddress<< " upstreamSocket = "<<a_upstreamSocket;
    m_addr = a_addr;
    m_gw = a_gw;
    m_upstreamAddress = a_upstreamAddress;
    m_upstreamSocket=a_upstreamSocket;
    tunDeviceCreate();
}

void DapTunAbstract::workerStart()
{
    onWorkerStarted();
    qInfo() <<"Tun device created";
    if(m_tunSocket>0){
        tunWorker->setTunSocket(m_tunSocket);
        tunWorker->moveToThread(tunThread);
        tunThread->start();
    }else{
        emit error(tr("Can't open tun socket"));
    }
}

/**
 * @brief DapTunAbstract::destroy
 */
void DapTunAbstract::destroy()
{
    if(m_tunSocket)
        workerStop();
    else
        qWarning() << "Tunnel is not working";
}


/**
 * @brief DapTunAbstract::onWorkerStarted
 */
void DapTunAbstract::onWorkerStarted()
{
    qInfo()<<"Worker loop started";
}

/**
 * @brief DapTunAbstract::onWorkerStopped
 */
void DapTunAbstract::onWorkerStopped()
{
    qInfo()<<"Worker loop stopped";
    tunWorker->moveToThread(this->thread());
    tunWorker->setTunSocket(-1);
    tunDeviceDestroy();
}


/**
 * @brief DapTunAbstract::afterTunDeviceDestroyed
 * @details Clear everything after tunnel destroy
 */
void DapTunAbstract::afterTunDeviceDestroyed()
{
    qInfo() <<"Tun device destroyed";
    m_addr.clear();
    m_gw.clear();
    m_gwOld.clear();
    m_upstreamAddress.clear();
    m_upstreamSocket=0;
    m_tunSocket=0;
}

/**
 * @brief DapChSockForw::saveNetworkInformationToFile
 */
void DapTunAbstract::saveNetworkInformationToFile()
{
    /*
    QDomDocument doc(tempNetFileName);
    QDomElement root = doc.createElement(tempNetFileName);
    doc.appendChild(root);

    QDomElement tag;

    auto addElementToFile = [&] (QString name_element, QString value) {
        tag = doc.createElement(name_element);
        root.appendChild(tag);
        tag.appendChild( doc.createTextNode(value) );
    };

    addElementToFile("tunAddr", m_tunAddr);
    addElementToFile("tunDest", m_tunDest);
    addElementToFile("ip_host", DapSession::getInstance()->get_address());
    addElementToFile("default_gw", m_defaultGwOld);
    addElementToFile("date_create", QDateTime::currentDateTime().toString("yyyy.M.d h:m"));

    QFile file(tempNetFileName);

    if(file.open(QIODevice::WriteOnly)) {
        QTextStream(&file) << doc.toString();
        file.close();
    }
    else
        qWarning() << "Error Creating File!";
*/
}

int parseTextToInt(QString text, int a, int b, char splitChar)
{
    return text.split(' ')[a].split(splitChar)[b].toInt();
}

/**
 * @brief DapTunAbstract::readNetrowkInformFromFile
 * @details Restores network settings from the file
 */
void DapTunAbstract::readNetrowkInformFromFile()
{
    QFile file(tempNetFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error open " << tempNetFileName;
        return;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if(xml.name() == "tunAddr") {
            xml.readNext();
            qDebug() << "tunAddr = " << xml.text();
            xml.readNext();
        } else if(xml.name() == "tunDest") {
            xml.readNext();
            qDebug() << "tunDest = " << xml.text();
            xml.readNext();
        } else if(xml.name() == "ip_host") {
            xml.readNext();
            qDebug() << "ip_host = " << xml.text();
            xml.readNext();
        } else if(xml.name() == "default_gw") {
            xml.readNext();
            qDebug() << "default_gw = " << xml.text();
            xml.readNext();
        } else if(xml.name() == "date_create") {
            xml.readNext();
            QString strDate = xml.text().toString();
            int year  = parseTextToInt(strDate, 0, 0, '.');
            int month = parseTextToInt(strDate, 0, 1, '.');
            int day   = parseTextToInt(strDate, 0, 2, '.');
            int hours = parseTextToInt(strDate, 1, 0, ':');
            int min   = parseTextToInt(strDate, 1, 1, ':');
            QDateTime dt(
                QDate(year, month, day),
                QTime(hours, min));

            qDebug() << "Differense(sec) = " << dt.secsTo(QDateTime::currentDateTime());
            xml.readNext();
        }
    }

    file.close();
}

bool DapTunAbstract::isLocalAddress(const QString& address)
{
    //In accordance with the IANA standard
    QStringList octets = address.split('.');
    if(octets.size() < 4)
        return false;
    int firstOctet = octets[0].toInt();
    int secondOctet = octets[1].toInt();
    if(firstOctet == 10)
        return true;
    else if(firstOctet == 172 && secondOctet >= 16 && secondOctet < 32)
        return true;
    else if(firstOctet = 192 && secondOctet == 168)
        return true;
    return false;

}
