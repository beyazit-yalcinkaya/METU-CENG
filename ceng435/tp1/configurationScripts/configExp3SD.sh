r1=$(getent ahosts "r1" | cut -d " " -f 1 | uniq)
r2=$(getent ahosts "r2" | cut -d " " -f 1 | uniq)
r3=$(getent ahosts "r3" | cut -d " " -f 1 | uniq)

r1_adapter=$(ip route get $r1 | grep -Po '(?<=(dev )).*(?= src| proto)')
r2_adapter=$(ip route get $r2 | grep -Po '(?<=(dev )).*(?= src| proto)')
r3_adapter=$(ip route get $r3 | grep -Po '(?<=(dev )).*(?= src| proto)')

sudo tc qdisc add dev $r1_adapter root netem delay 50ms 5ms distribution normal
sudo tc qdisc add dev $r2_adapter root netem delay 50ms 5ms distribution normal
sudo tc qdisc add dev $r3_adapter root netem delay 50ms 5ms distribution normal
