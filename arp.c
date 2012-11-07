#include "arp.h"

struct ArpPacket
{
	unsigned char HWType[2];
	unsigned char ProtocolType[2];
	unsigned char HWAddLength;
	unsigned char ProtocolAddLength;
	unsigned char Operation[2];
   unsigned char SenderHWAddress[6];
	unsigned char SenderProtocolAddress[4];
	unsigned char TargetHWAddress[6];
	unsigned char TargetProtocolAddress[4];
};

typedef struct ArpPacket ArpPacket_t;


void DumpArpPacket(ArpPacket_t *data)
{
	printf("Dumping Arp packet\n");
	uint16 hwType = data->HWType[0]<<8 |  data->HWType[1];
	switch (hwType){
		case 1: printf("HW type is ethernet\n");
				  break;
		default : printf("Unidentified HW type\n");
				  break;
	}
	uint16 protocolType = data->ProtocolType[0] << 8 | data->ProtocolType[1];
	switch (protocolType){
		case 0x0800: printf("Protocol type is IPv4\n");
			break;
		default : printf("Unknown protocol type\n");
			break;
	}
	uint16 operation = data->Operation[0]<<8 | data->Operation[1];
	printf("Operation  Type = ");
	switch(operation){
		case 1: printf(" Arp Request\n");
			break;
		case 2: printf(" Arp Reply\n");
			break;
		case 3: printf(" RArp Request\n");
			break;
		case 4: printf(" RArp Reply\n");
			break;
		case 5: printf(" DRArp Request\n");
			break;
		case 6: printf(" DRArp Reply\n");
			break;
		default : printf("Unkonwn Operation Type\n");
			break;
	}
	
	int i;	
	printf("Sender HW address = ");
	for(i=0;i<data->HWAddLength;i++){
		printf("%x ",data->SenderHWAddress[i]);
	}
	printf("\n");
	printf("Sender Protocol address = ");
	for(i=0;i<data->ProtocolAddLength;i++){
		printf("%d ",data->SenderProtocolAddress[i]);
	}
	printf("\n");
	printf("Target HW address = ");
	for(i=0;i<data->HWAddLength;i++){
		printf("%x ",data->TargetHWAddress[i]);
	}
	printf("\n");
	printf("Target Protocol address = ");
	for(i=0;i<data->ProtocolAddLength;i++){
		printf("%d ",data->TargetProtocolAddress[i]);
	}
	printf("\n");

}

void HandleArpPacket(char *data)
{
	ArpPacket_t *packet = ( ArpPacket_t *)data;
	DumpArpPacket(data);
}
