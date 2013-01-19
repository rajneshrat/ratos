// This is for icmp support for ratos. Right now it only has ping feature
#include "icmp.h"
#include "common.h"
#include "screen.h"

struct IcmpPacket {
    unsigned char type;
    unsigned char code;
    unsigned char checksum[2];
    unsigned char data[4];
};

typedef struct IcmpPacket IcmpPacket_t;

void DumpIcmpPacket(IcmpPacket_t *packet)
{
    printk("Dumping Icmp Packet\n");
    printk("Type = %x\n", packet->type);
    printk("Code = %x\n", packet->code);
    int i;
    for(i=0; i<4; i++) {
        printk("%x", packet->data[i]);
    }
}

uint16 CalculateChecksum(unsigned char *data, int len)
{
    uint8 *val = data;
    int i;
    uint32 sum = 0;
    for(i=0; i<len/2; i++) {
        uint16 val16 = val[0] & 0xff;
        val16 = val16 << 8 | val[1];
        val = val + 2;
        sum = val16 + sum;
    }
    while( sum & 0xffff0000) {
        sum =  (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

void HandlePingReply(IcmpPacket_t *packet, char *srcIP)
{
	printf("Received ping reply from ");
	printf("%d",srcIP[0]);
	int i;
	for(i=1;i<4;i++){
		printf(".%d",srcIP[i]);
	}
	printf("\n");
}
		
void HandleIcmpPacket(unsigned char *data, unsigned char *srcIP, unsigned char *dstIP)
{
    IcmpPacket_t *packet = (IcmpPacket_t *) data;
    //SendPingReply(dstIP, srcIP);
    packet->type = 0;
    packet->code = 0;
    packet->checksum[0] = 0;
    packet->checksum[1] = 0;
    uint16 checksum = CalculateChecksum(data, 32);
	int i;
	for(i=0;i<50;i++){
		packet->data[i] = 0;
	}
  //  packet->checksum[0] = checksum >> 8;
  //  packet->checksum[1] = checksum & 0xff;
//	DumpIcmpPacket(packet);
    switch( packet->type){
		case 0: HandlePingReply(packet, srcIP);
			break;
		case 8: SendPingReply(dstIP, srcIP);
			break;
	}
//	
  SendIpv4Packet(packet, 10, 0x1, dstIP, srcIP);
}

void SendPingReply(unsigned char *srcIP, unsigned char *dstIP)
{
    IcmpPacket_t *packet = imalloc(sizeof(IcmpPacket_t));
    packet->type = 0;
    packet->code = 0;
    SendIpv4Packet(packet, 64, 1, srcIP, dstIP);
}

void SendPingRequest(unsigned char *srcIP, unsigned char *dstIP)
{
    IcmpPacket_t *packet = imalloc(sizeof(IcmpPacket_t));
    packet->type = 8;
    packet->code = 0;
    packet->checksum[0] = 0;
    packet->checksum[1] = 0;
    int i;
    for(i=0; i<4; i++) {
        printk("%d ", dstIP[i]);
    }
    uint16 checksum = CalculateChecksum(packet, 32);
    packet->checksum[0] = checksum >> 8;
    packet->checksum[1] = checksum & 0xff;
    SendIpv4Packet(packet, 64, 1, srcIP, dstIP);
}
