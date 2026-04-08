#!/bin/bash
echo "----------Compile--------"
gcc -o 1 1.c -lpthread
echo
echo "----------Creating Files----------"
# cat /dev/random | head -c <bytecount>
cat /dev/random | head -c 100000 > src_1.dat
cat /dev/random | head -c 100000 > src_2.dat
cat /dev/random | head -c 100000 > src_3.dat
cat /dev/random | head -c 100000 > src_4.dat
cat /dev/random | head -c 100000 > src_5.dat
cat /dev/random | head -c 100000 > src_6.dat
cat /dev/random | head -c 100000 > src_7.dat
cat /dev/random | head -c 100000 > src_8.dat
cat /dev/random | head -c 100000 > src_9.dat
cat /dev/random | head -c 100000 > src_10.dat
echo

echo  "----------Running----------"
echo "->Multithreaded copy vs single threaded copy..."
./1 src_1.dat src_2.dat src_3.dat src_4.dat src_5.dat src_6.dat src_7.dat src_8.dat src_9.dat src_10.dat
echo``
rm *.dat*
echo