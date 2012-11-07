#include "ethernet.h"
#include "arp.h"

struct EthernetPacket
{
	uint32 Preamble;
	unsigned char DstMac[6];
	unsigned char SrcMac[6];
	unsigned char Type[2];
	unsigned char Data;  //this is just to get the address of stream. &Data
};
typedef struct EthernetPacket EthernetPacket_t;

void PushPacketToEth(char *rawData)
{
	EthernetPacket_t *Packet =(EthernetPacket_t*)rawData;
	printf("Type = %x\n",Packet->Type[0]);
   uint16 Type = Packet->Type[0] << 8 | Packet->Type[1];
	switch (Type){
		case 0x0800:	printf("Detected IPv4 Packet\n");
							break;
		case 0x0806:	printf("Arp Packet\n");
                     HandleArpPacket(&Packet->Data);
							break;
		default :	printf("Unknown Ethernet Packet\n");
	}
}

