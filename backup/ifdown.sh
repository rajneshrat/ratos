#qemu-ifdown
ifconfig eth0 down
ifconfig eth0 -promisc
ifup eth0
ifconfig br0 down
brctl delbr br0
openvpn --rmtun --dev tap0
