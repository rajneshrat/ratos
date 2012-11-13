#!/bin/bash
#make &&
./update_image.sh 

brctl addbr br0
tunctl -t tap0 -u root2
brctl addif br0 eth0
brctl addif br0 tap0
ifconfig eth0 up
ifconfig tap0 up
ifconfig br0 10.0.2.21/24
ifconfig eth0 0.0.0.0 promisc
#kvm -m 1G -fda floppy.img -net nic,macaddr=00:aa:00:60:21:01,model=virtio -net tap,ifname=tap0,script=no
#kvm -m 1G -fda floppy.img -net nic,macaddr=00:aa:00:60:21:01,model=rtl8139 -net tap,ifname=tap0,script=no
#qemu-system-i386 -m 1G -kernel kernel -net nic,macaddr=00:aa:00:60:21:01,model=rtl8139 -net tap,ifname=tap0,script=no
#qemu -m 1G -fda floppy2.img -net nic,macaddr=00:aa:00:60:21:01,model=rtl8139 -net tap,ifname=tap0,script=no
qemu -m 1G -kernel kernel  -net nic,macaddr=00:aa:00:60:21:01,model=rtl8139 -net tap,ifname=tap0,script=no
ifconfig br0 down
brctl delbr br0



#another example
#sh ../ifdown.sh
#sh ../ifup.sh
#qemu -hda ../../linuxdisk/linux-0.2.img -kernel arch/x86/boot/bzImage -net nic -net tap,ifname=tap0,script=no -append root=/dev/sda
