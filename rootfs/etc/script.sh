#!/bin/sh
ls
ps
cd /rootfs/etc/
echo $PS1
echo $PATH
sleep 2
pwd
ulimit
cat /rootfs/etc/script.sh
./init
/rootfs/bin/init
sleep 5 &
ps
