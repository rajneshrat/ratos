#include "icmp.h"
#include "common.h"
#include "screen.h"

struct IcmpPacket{
	unsigned char type;
	unsigned char code;
	unsigned char checksum[2];
	unsigned char data[4];
};

typedef struct IcmpPacket IcmpPacket_t;

void DumpIcmpPacket(IcmpPacket_t *packet)
{
	printf("Dumping Icmp Packet\n");
	printf("Type = %x\n", packet->type);
	printf("Code = %x\n", packet->code);
	int i;
	for(i=0;i<4;i++){
		printf("%x", packet->data[i]);
	}
}

uint16 CalculateChecksum(unsigned char *data, int len)
{
	uint8 *val = data;
	printf("Checksum\n");
	int i;
	uint32 sum = 0;
	for(i=0;i<len/2;i++){
		uint16 val16 = val[0] & 0xff;
		val16 = val16 << 8 | val[1];
		printf("%x ",val16);
		val = val + 2;
		sum = val16 + sum;
	}
	while( sum & 0xffff0000){
		sum =  (sum & 0xffff) + (sum >> 16);
	}
	printf("\n tot = %x\n",~sum);
	return ~sum;
}

void HandleIcmpPacket(unsigned char *data, unsigned char *srcIP, unsigned char *dstIP)
{
	IcmpPacket_t *packet = (IcmpPacket_t *) data;
   //SendPingReply(dstIP, srcIP);
	packet->type = 0;
	packet->code = 0;
	packet->checksum[0] = 0;
	packet->checksum[1] = 0;
	uint16 checksum = CalculateChecksum(data, 64);
	packet->checksum[0] = checksum >> 8;
	packet->checksum[1] = checksum & 0xff;
	printf("Check %x\n",checksum);
//    SendPingReply(dstIP, srcIP);
//	SendIpv4Packet(packet, 10, 0x1, dstIP, srcIP);
//	DumpIcmpPacket(packet);	
}

void SendPingReply(unsigned char *srcIP, unsigned char *dstIP)
{
	IcmpPacket_t *packet = imalloc(sizeof(IcmpPacket_t));
	packet->type = 0;
	packet->code = 0;
//	packet.type = 0;
//	packet.code = 0;
	int i;
	printf("IP isss \n");	
	for(i=0;i<4;i++){
		printf("%d ", dstIP[i]);
	}
	SendIpv4Packet(packet, 10, 1, srcIP, dstIP);
}

