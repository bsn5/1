#include "staticdata.h"
#include <QHostAddress>

StaticData* StaticData::m_pinstance = 0;
StaticData* StaticData::me() {
  if(m_pinstance == 0)
     m_pinstance = new StaticData;
  return m_pinstance;
}

void StaticData::sendPacketOut(DapSockForwPacket *pktOut)
{
    emit packetOut(pktOut);
}

DapSockForwPacket *StaticData::getPacket()
{
    if(tun)
        return tun->writeDequeuePacket();
    return nullptr;
}

StaticData::StaticData(): QObject(nullptr)
{

}
