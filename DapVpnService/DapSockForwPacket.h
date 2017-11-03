#ifndef DAPSOCKFORWPACKET_H
#define DAPSOCKFORWPACKET_H

#include <QtGlobal>
struct DapSockForwPacketHdr{
    int socket_id;
    quint32 op_code;
    union
    {
        struct
        {
            quint32 addr_size;
            quint16 port;
            quint16 padding;
        } op_connect;
        struct
        {
            quint32 data_size;
            quint32 padding;
        } raw;   // Default access to raw OP data
        struct
        {
            quint32 data_size;
            quint32 padding;
        } op_data;
    };
} __attribute__((packed));

struct DapSockForwPacket
{
    DapSockForwPacketHdr header;
    quint8 data[];
} __attribute__((packed));

#endif // DAPSOCKFORWPACKET_H
