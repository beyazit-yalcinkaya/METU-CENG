r1=$(getent ahosts "r1" | cut -d " " -f 1 | uniq)
r2=$(getent ahosts "r2" | cut -d " " -f 1 | uniq)
r3=$(getent ahosts "r3" | cut -d " " -f 1 | uniq)

r1_adapter=$(ip route get $r1 | grep -Po '(?<=(dev )).*(?= src| proto)')
r2_adapter=$(ip route get $r2 | grep -Po '(?<=(dev )).*(?= src| proto)')
r3_adapter=$(ip route get $r3 | grep -Po '(?<=(dev )).*(?= src| proto)')

sudo tc qdisc del dev $r1_adapter root
sudo tc qdisc del dev $r2_adapter root
sudo tc qdisc del dev $r3_adapter root
