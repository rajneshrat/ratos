#include "ethernet.h"
#include "arp.h"

static char HostHWAdd[6] = { 0x00,0xaa,0x00,0x60,0x21,0x01 };

struct EthernetPacket
{
    uint32 Preamble;
    unsigned char DstMac[6];
    unsigned char SrcMac[6];
    unsigned char Type[2];
    unsigned char Data[100];  //this is just to get the address of stream. &Data
};
typedef struct EthernetPacket EthernetPacket_t;

void ReceivePacketFromWire(char *rawData)
{
    EthernetPacket_t *Packet =(EthernetPacket_t*)rawData;
    uint16 Type = Packet->Type[0] << 8 | Packet->Type[1];
    switch (Type) {
    case 0x0800:
        AddKernelMesg("Detected IPv4 Packet\n");
        HandleIpv4Packet(Packet->Data);
        break;
    case 0x0806:
        AddKernelMesg("Arp Packet\n");
        HandleArpPacket(Packet->Data);
        break;
    default :
        AddKernelMesg("Unknown Ethernet Packet\n");
    }
}

void SendPacketToCard(char dstHwAdd[6], unsigned char *rawData, uint32 length, uint16 type)  //length is in bytes
{
    EthernetPacket_t *packet = imalloc(sizeof(EthernetPacket_t));
    int i;
    for(i=0; i<6; i++) {
        packet->DstMac[i] = dstHwAdd[i];
    }
    for(i=0; i<6; i++) {
        packet->SrcMac[i] = HostHWAdd[i];
    }
    packet->Type[0] = type >> 8 & 0xff;
    packet->Type[1] = type & 0xff;
    copy(packet->Data, rawData, length);
    SendPacketOut(&packet->DstMac[0], length+sizeof(EthernetPacket_t) - 4 - sizeof(packet->Data) ); // there is no premble while sending
}
