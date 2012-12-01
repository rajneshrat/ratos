#include "demo.h"
#include <mem.h>
unsigned int
CopyFromPacketToBuffer(
	PPACKET	      pPacket,
	unsigned char *pBuffer
)
{
	unsigned char	bufferCount;
	unsigned int    offset = 0;
	PBufferList	pBufList;
	for(pBufList=&(pPacket->Buffers) , bufferCount = 0;
		bufferCount < pPacket->BufferCount;
		bufferCount++)
	{
		memcpy(pBuffer+offset , pBufList->Buffer , pBufList->BufferLength);
		offset += pBufList->BufferLength;
		pBufList = pBufList->Next;
	}
	return offset;
}