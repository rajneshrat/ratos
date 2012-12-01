
unsigned char CardHwAdd[5];
void SendPacketOut(unsigned char *packetData, uint32 len);
void InitializeRtl8139(uint8 busNumber, uint8 deviceNumber);
inline char *GetCardHwAdd() {
    return CardHwAdd;
}
