#!/bin/bash

echo "in bash"

#output path
echo $1

#job number
echo $2

#level number
echo $3

#input file
echo $4


# merge ( inputFile, jobNumber, levelNumber, outputPath)

root -l -b -q 'mergeTree.C('\"$4\"', '$2', '$3', '\"$1\"')'
echo "done"


mv *.root $1
