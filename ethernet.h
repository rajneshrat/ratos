#ifndef __ETHERNET.H__
#define __ETHERNET.H__
#include "common.h"
#include "screen.h"
void ReceivePacketFromWire(char *rawData);
void SendPacketToCard(char dstHwAdd[6], unsigned char *rawData, uint32 length, uint16 type);  //length is in bytes
#endif
