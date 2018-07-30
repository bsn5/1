 #ifndef DAPTUNABSTRACT_H
#define DAPTUNABSTRACT_H

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QReadWriteLock>
#include <pthread.h>


#include "DapTunWorkerAbstract.h"
#include "DapSockForwPacket.h"

class DapTunAbstract : public QObject
{
    Q_OBJECT
public:
    DapTunAbstract();

    void create(const QString& a_addr, const QString& a_gw,  const QString & a_upstreamAddress, int a_upstreamSocket );
    void destroy();

    void setTunSocket(int a_tunSocket){ m_tunSocket = a_tunSocket; }

    bool isCreated();
    const QString& addr() { return m_addr; }
    const QString& gw() { return m_gw; }
    int upstreamSocket() { return m_upstreamSocket; }
    const QString & upstreamAddress() { return m_upstreamAddress; }
    const QString& tunDeviceName(){ return m_tunDeviceName; }

    void tunWriteData(DapSockForwPacket * a_pkt){
        addWriteData(a_pkt);
        signalWriteQueueProc();
    }

    virtual void workerStart(); // В основном для мобильных платформ, где тун девайс открывается через задницу

    QQueue<DapSockForwPacket*>* writeQueue(){ return &m_writeQueue; }
    QReadWriteLock* writeQueueLock(){ return &m_writeQueueLock; }
    QWaitCondition * writeQueueCond() { return &m_writeQueueCond; }

    int m_tunSocket;

    DapSockForwPacket * writeDequeuePacket() {
        DapSockForwPacket * ret;
        m_writeQueueLock.lockForRead();
        if (m_writeQueue.length() > 0) {
            ret = m_writeQueue.dequeue();
        } else {
            ret = nullptr;
        }
        m_writeQueueLock.unlock();
        return ret;
    }

signals:
    void nativeCreateRequest();
    void nativeDestroyRequest();

    void created();
    void destroyed();
    void error(const QString&);

    void sendCmd(QString);

    void packetOut( DapSockForwPacket * );
protected:
    virtual void tunDeviceCreate()=0;
    virtual void tunDeviceDestroy()=0;

    virtual void workerPrepare()=0;
    virtual void workerStop()=0;

    virtual void signalWriteQueueProc()=0;

    void initWorker();
    void saveNetworkInformationToFile();
    void readNetrowkInformFromFile();
    bool isLocalAddress(const QString &address);

    QString m_gwOld;
    DapTunWorkerAbstract * tunWorker;
    QThread * tunThread;


    QString m_tunDeviceName; // tunDevice name
    QString m_defaultGwOld;
    QStringList m_defaultDNSRecord;

private:
    const QString tempNetFileName = "TempConfigurationNetwork.xml";

    bool m_isCreated;
    int m_upstreamSocket;
    int m_MTU;
    pthread_t pidTun;


    QQueue<DapSockForwPacket*> m_writeQueue;
    QReadWriteLock m_writeQueueLock;
    QWaitCondition m_writeQueueCond;


protected:
    QString m_addr, m_gw, m_upstreamAddress;

protected slots:
    void addWriteData(DapSockForwPacket* a_pkt){
        m_writeQueueLock.lockForWrite();
        m_writeQueue.enqueue(a_pkt);
        m_writeQueueLock.unlock();
    }

    virtual void onWorkerStarted();
    virtual void onWorkerStopped();

    virtual void afterTunDeviceDestroyed();
};

#endif // DAPTUNABSTRACT_H
