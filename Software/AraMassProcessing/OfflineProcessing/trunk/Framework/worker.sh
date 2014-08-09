#!/bin/bash

echo "in bash"

#input file
echo $1

#job number
echo $2

#level number
echo $3

#flag for arasim or merge
echo $4

#flag 0 for arasim
#flag 1 for merging

if [ $4 = 0 ]; then
    echo "Running arasim...."
    tar -zxf archive.tar.gz
    ls -l
    ./AraSim setup.txt $2 ./
else
    echo "Merging..."
    root -l -b -q 'mergeTree.C('\"$1\"', '$2', '$3')'
fi
echo "done"


mv *.root /home/cbora/dev/AraSim/Temp/

#mv * /home/cbora/dev/AraSim/ToDelete/

rm batchScript_test*
rm *.cc
rm *.o
rm *.h
rm *.hh
rm *f
rm setup*
rm ARA*
rm *.txt
rm M*
rm ray_*
rm read*
rm runaraSim*
rm README_*
rm eventSim*
rm DT_*
rm archive*
rm AraSim
rm Standard*
rm library.sh
