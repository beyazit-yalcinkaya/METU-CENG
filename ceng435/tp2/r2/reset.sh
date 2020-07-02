s=$(getent ahosts "s" | cut -d " " -f 1 | uniq)
d=$(getent ahosts "d" | cut -d " " -f 1 | uniq)

s_adapter=$(ip route get $s | grep -Po '(?<=(dev )).*(?= src| proto)')
d_adapter=$(ip route get $d | grep -Po '(?<=(dev )).*(?= src| proto)')

sudo tc qdisc del dev $s_adapter root
sudo tc qdisc del dev $d_adapter root
