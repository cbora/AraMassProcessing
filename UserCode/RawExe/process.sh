#ls -l
#echo "IN bash shifhashjkhasklhfkjashfkjhsajdkfhdsakjhfkadjshajl "
source /net/local/ara/repositories/setupScripts/AraRoot_v3.9.sh               #// needed for latest version of AraRoot

echo "In Process.sh script "
echo "list of files to process: "   $1  #contains list of files to be processed
echo "Path for storing log files: " $2  #path for storing log files
echo "Section number: "             $3  #section number
echo "Results output path: "        $4  #output path for Results
#export DIR=/home/cbora/DataProcessing/ #// location of raw data 
#export DIR=/data/exp/ARA/2011/minbias/SPS-ARA/Testbed_2011/event/
#i=*006020     #// just as an example run (this would be your loop usually)

#find $i -name "*.dat" > fileList.txt
sleep 20
#/net/user/hagar/ARA2/blind/
temp=$2'temp'$3'.txt'
errlog=$2'errorlog'$3'.txt'

echo $temp
echo $errlog
#./make01 $1 a.root 0 10 1 | grep "start "> temp_log.txt   #// this is Hagar's script 
./make01 $1 a.root 0 10 1 | grep "start "> $temp   #// this is Hagar's script 
#/home/cbora/Misc/

#o=$2
o=$4  #path for storing output files
a='fjalkja'
#out=$o$a
#echo $out
#awk -v x=$o '{print "mkdir -p "x$4" && mv *.run"$2".root* "x$4"/." | "/bin/sh"}' temp_log.txt 2>> errorlog.txt  #// make dir's for files
awk -v x=$o '{print "mkdir -p "x$4" && mv *.run"$2".root* "x$4"/." | "/bin/sh"}' $temp 2>> $errlog  #// make dir's for files

#echo $o
echo "DONE"
#cat temp_log.txt >> log.txt   #// just for logging everything. 
cat $temp >> log.txt   #// just for logging everything. 

#rm *.root*