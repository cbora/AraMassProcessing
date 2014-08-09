#!/bin/bash

#output path file
echo $1

#job number
echo $2

./AraSim setup.txt $2 ./


if [ $? -eq 0 ]; then 
    echo "Jobs successful - exit code = 0"
    mv *.root $1
else
    echo " Jobs Failure exit code non-zero --- Removing Output files"
    rm *.root

fi

echo "Exit code: "
echo $?

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
