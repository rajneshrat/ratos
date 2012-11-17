#include "ipv4.h"
#include "common.h"
#include "screen.h"

struct Ipv4Packet {
    unsigned char version:4;
    unsigned char Ihl:4;
    unsigned char ToS;
    unsigned char length[2];
    unsigned char identification[2];
    unsigned char Flags:3;
    unsigned char FragmentOffestHigh:5;
    unsigned char FragmentOffestLow;
    unsigned char TTL;
    unsigned char Protocol;
    unsigned char HeaderChecksum[2];
    unsigned char SrcIP[4];
    unsigned char DstIP[4];
    unsigned char data[100];
};

typedef struct Ipv4Packet Ipv4Packet_t;

/*
uint16 CalculateChecksum(unsigned char *data, int len)
{
    uint8 *val = data;
    int i;
    uint32 sum = 0;
    for(i=0; i<len/2; i++) {
        uint16 val16 = val[0] & 0xff;
        val16 = val16 << 8 | val[1];
		val[0] = 0; val[1] = 0; // delete me
        val = val + 2;
        sum = val16 + sum;
    }
    while( sum & 0xffff0000) {
        sum =  (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}
*/
void DumpIpv4Packet(Ipv4Packet_t *packet)
{
    printf("Version = %x\n", packet->version);
    printf("Ihl = %x\n", packet->Ihl);
    printf("Tos = %x\n", packet->ToS);
    
	printf("Length High= %d\n", packet->length[0]);
	printf("Length Low= %d\n", packet->length[1]);
    uint16 identy = packet->identification[0] << 8 | packet->identification[1];
    printf("Identification = %x\n", identy);
    printf("Flags = %x\n", packet->Flags);
    uint16 fragmentOffset = packet->FragmentOffestHigh << 5 | packet->FragmentOffestLow;
    printf("FragmentationOffset Low = %x\n", packet->FragmentOffestLow);
    printf("FragmentationOffset High = %x\n", packet->FragmentOffestHigh);
    printf("FragmentationOffset = %x\n", fragmentOffset);
    printf("Ttl = %x\n", packet->TTL);
    printf("Protocol = %x\n", packet->Protocol);
    int i;
    printf("Src IP = ");
    for(i=0; i<4; i++) {
        printf("%d ",packet->SrcIP[i]);
    }
    printf("Dst IP = ");
    for(i=0; i<4; i++) {
        printf("%d ",packet->DstIP[i]);
    }
}

//unsigned char TargetHWAdd[6] = { 0xe8, 0x03, 0x9a, 0x03, 0xf0, 0x11 };
//unsigned char TargetHWAdd[6] = { 0xaa, 0x7d, 0x7c, 0xc0, 0x10, 0x9d };

void HandleIpv4Packet(unsigned char *data)
{
    Ipv4Packet_t *packet = (Ipv4Packet_t *)data;
	DumpIpv4Packet(packet);
    switch(packet->Protocol) {
    case 1:
        printf("This is a Icmp packet\n");
        // 	SendPacketToCard(TargetHWAdd, packet, 64 + sizeof(Ipv4Packet_t) - 100, 0x0800);
        HandleIcmpPacket(packet->data, packet->SrcIP, packet->DstIP);
        packet->identification[0] = 0x00; //  this is the identification field, yet to be implemented.
        //packet->identification[1] = 0x31; //  this is the identification field, yet to be implemented.
	    int i;
        unsigned char temp;
        for(i=0; i<4; i++) {
            temp = packet->SrcIP[i];
            packet->SrcIP[i] = packet->DstIP[i];
            packet->DstIP[i] = temp;
        }
        break;
    case 2:
        printf("This is a Igmp packet\n");
        break;
    case 6:
        printf("This is a Tcp packet\n");
        break;
    case 17:
        printf("This is a Udp packet\n");
        break;
    }
}

void SendIpv4Packet(unsigned char *data, uint16 len, uint8 protocol, unsigned char *srcIP, unsigned char *dstIP)
{
    Ipv4Packet_t *packet = (Ipv4Packet_t *) imalloc(sizeof(Ipv4Packet_t));
	
	packet->version = 5;// this is the current value of ipv4 keep this always.
    packet->Ihl = 4;  //size of header 4*32
    packet->ToS = 0; //  this decide QoS right now setting as 0 for default
    uint16 length = (len + sizeof(Ipv4Packet_t) - 100) ;
    packet->length[0] = 0;//length >> 8;
    packet->length[1] = 84;//length & 0x00ff;
    packet->identification[0] = 0x0; //  this is the identification field, yet to be implemented.
    packet->identification[1] = 0x0; //  this is the identification field, yet to be implemented.
    packet->Flags = 0;// to tell router whether fragment or not.
    packet->TTL = 0x40; // maximum no of routers before it get dropped
    packet->Protocol = 1;//protocol; // 1 for icmp , ....
    packet->FragmentOffestHigh = 0x8;
    packet->FragmentOffestLow = 0;
    int i;
    for(i=0; i<4; i++) {
        packet->SrcIP[i] = srcIP[i];
    }
    for(i=0; i<4; i++) {
        packet->DstIP[i] = dstIP[i];
    }
    packet->HeaderChecksum[0] = 0;
    packet->HeaderChecksum[1] = 0;
    uint16 checksum = CalculateChecksum(packet, 20);
    packet->HeaderChecksum[0] = checksum >> 8;
    packet->HeaderChecksum[1] = checksum & 0xff;
    copy(packet->data, data, 100);
    unsigned char TargetHWAdd[6];
    GetMacAddressForIP(packet->DstIP, TargetHWAdd);
  //  SendPacketToCard(TargetHWAdd, packet, len + sizeof(Ipv4Packet_t) - 100, 0x0800);
    SendPacketToCard(TargetHWAdd, packet, 64 + sizeof(Ipv4Packet_t) - 100, 0x0800);
//	DumpIpv4Packet(packet);
}
