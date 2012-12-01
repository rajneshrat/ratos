#qemu-ifup
ifconfig eth0 down
ifconfig eth0 0.0.0.0 promisc up
openvpn --mktun --dev tap0
ifconfig tap0 0.0.0.0 up
brctl addbr br0
brctl addif br0 eth0
brctl addif br0 tap0
brctl stp br0 off
ifconfig br0 10.0.2.21 netmask 255.255.255.0
