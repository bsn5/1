#ifndef STATICDATA_H
#define STATICDATA_H

#include <QObject>
#include "DapSockForwPacket.h"
#include <QTcpSocket>
#include "DapTunAbstract.h"



class StaticData : public QObject{
    Q_OBJECT
public:
  static StaticData* me();

  void sendPacketOut(DapSockForwPacket * pktOut);

  DapSockForwPacket* getPacket();

  void setTun(DapTunAbstract* tun_){
      tun = tun_;
  }


//...
private:
  QTcpSocket sock;
  static StaticData* m_pinstance;
  DapTunAbstract* tun;

  StaticData();

signals:
  void packetOut(DapSockForwPacket *);
};

#endif // STATICDATA_H
