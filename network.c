#include "pci.h"
#include "network.h"
#include "screen.h"

//   for RTL8139  network driver -----
void DetectRTL8139()
{
    uint16 vendorid = 0x10ec;
    uint16 deviceid = 0x8139;
    int present = checkpcitable(deviceid, vendorid);
    if(present)
    {
        printf(" Detected RTL8139 Network Driver\n");
    }
}

void DetectNetworkDevices()
{
    DetectRTL8139();
}
