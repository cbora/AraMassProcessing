#!/usr/bin/perl

#use strict;
#use warnings;
#use diagnostics;
use File::Basename;
# This script executes separate physics analysis executable on each given file.
print "Shell script to be run on worker nodes\n";
print "*** command line arguments ***\n";
# arg1 - job*.txt - number of the file
# arg2 - full directory path where to store the results

#output path
$arg1 = shift;
#job number
$arg2 = shift;
#level number in case of merging
$arg3 = shift;

#flag for arasim or merger
$arg4 = shift;

#input file in case of merger
$arg5 = shift;
print "arg1 = $arg1, arg2 = $arg2, arg3 = $arg3, arg4 = $arg4, arg5 = $arg5  \n";

#Unzip files used in workerScript
`tar -zxf archive.tar.gz`;

print "*** date ***\n";
print `date`;
print "*** hostname ***\n";
print `hostname`;
print "Files on the submit node *** ls -1 ***\n";
print `ls -1`;

print "ls -l done \n";
#do 'xmlStatistics.pl';	# Module of creating xml-stat files

# Get status of the process with pid
sub ProcessRunning{
	my $pid = @_[0];
	my $status = kill 0, $pid;
	print "." if( $status);
	if ($status == 0) {
	    #print "process not running\n";
		return 0;
	} else {
	    #print "process running\n";
		return 1;
	}
}

# Waits for maximum of $maxWait and terminate after that
# If the process is finished normally return 0 otherwise 1
sub WaitAndTerminate {
#This function is not used anymore waitpid() is used instead
	my $pid = @_[0];

	print "Waiting for process pid = $pid\n";
	my $waitSec = 1;
	my $numSec = -$waitSec;
	#24 hours max to process a single file
	my $maxWait = 24 * 60 * 60;
	print"\n";
	    print "Process is still runnining";
       	while (ProcessRunning($pid)) {
		# The process is still running

		$numSec += $waitSec;
		if ($numSec > $maxWait) {
			# It is running for too long
			print "Process $pid is timed out. Terminating: " . (system("kill $pid") == 0? 'Done' : 'Failed') . "\n";
			return 1;
		}
		#print "sleeping\n";
		sleep $waitSec;
	}
	print "\n";
	print "number of seconds = $numSec\n";
return 0;
}

sub GetFirstLine {
    my $multiLine = @_[0];
    my @lines = split(/\n/, $multiLine);
    return @lines[0];
}


my $listFile = "$arg2/Input/job$arg1.txt";
#print "*** Running ROOT on each file from the list $listFile ***\n";

#my $stat = CreateStat("job$arg2.txt");

my $count = 0;
my $success = 0;
my $nonZeroExitCode = 0;
my $terminated = 0;
#use POSIX ":sys_wait_h";

if ( $arg4 == 0){
    #run arasim
    RunAraSim($arg1, $arg2);
}

if( $arg4 == 1){

    #run merger

    RunAraSimMerger($arg1, $arg2, $arg3, $arg5);
}

sub RunAraSim {
    #function to call bash script to execute arasim
    $outputPath = $_[0];
    $jobNumber = $_[1];
    
    #Redirect output and error files in the right directory
    $dir ="$outputPath/Logs/Physics/";
    $outputFile =$dir . "AraSimOut_" . $jobNumber . ".out";
    $errorFile =$dir . "AraSimOut_" . $jobNumber . ".err";
    $resultDir ="$argument2/Results/";
    print "output ". $outputFile;
    print "\n";
    print "error ". $errorFile;
    print "\n";
    $now = localtime;
    print "now = $now\n";
#    $dir ="$argument2/Results/";
    
    $outputPath = "$outputPath/Temp/";
    my $command = "./runAraSim.sh $outputPath $jobNumber 1>$outputFile 2>$errorFile & echo \$!";
    my $results =`$command`;
    my $pid = GetFirstLine($results);
    
    print "PID = $pid \n";
    if (WaitAndTerminate($pid)) { # Terminated                                                                                                                                             
#	AddValue($stat, {'name'=>"Timed out file name $terminated", 'digest'=>'copy', 'content'=>"$line"});
	`echo $?`;
	$terminated ++;
   } else { # Finished                                                                                                                                                                    
       `echo $?`;
	$success ++;
   }
    $count = $count + 1;
}


sub RunAraSimMerger {

    $outputPath = @_[0];
    $jobNumber = @_[1];
    $level = @_[2];
    $inputFile = @_[3];

    $outputPath = $outputPath;
    #Redirect output and error files in the right directory
    $dir ="$outputPath/Logs/MergeLogs/";
    $outputFile =$dir . "mergeLog_". $level . "_" . $jobNumber . ".out";
    $errorFile =$dir . "mergeLog_" . $level . "_" . $jobNumber . ".err";
    $resultDir ="$outputPath/Temp/";
    $now = localtime;
    print "now = $now\n";
#    $dir ="$argument2/Results/";
    
    $outputPathResults = "$outputPath/Temp/";
    my $command = "./runAraSimMerger.sh $outputPathResults $jobNumber $level $inputFile 1>$outputFile 2>$errorFile & echo \$!";
    my $results =`$command`;
    my $pid = GetFirstLine($results);
    
    print "PID = $pid \n";
    if (WaitAndTerminate($pid)) { # Terminated                                                                                                                                             
#	AddValue($stat, {'name'=>"Timed out file name $terminated", 'digest'=>'copy', 'content'=>"$line"});
	$terminated ++;
   } else { # Finished                                                                                                                                                                    
	$success ++;
   }
    $count = $count + 1;

}



sub ExeCmd{
    
    $listOfFile=$_[0];
    $argument2=$_[1];
    $argument1=$_[2];

    $now = localtime;
    print "now = $now\n";
    $dir ="$argument2/Results/";
    
    $pid = fork;
    unless($pid){
	exec(qw/sh process.sh/, $listOfFile, $dir, $argument1);
#    	if (WaitAndTerminate($pid)) { # Terminated
#		AddValue($stat, {'name'=>"Timed out file name $terminated", 'digest'=>'copy', 'content'=>"$listOfFile"});
#		$terminated ++;
#	} else { # Finished
#		$success ++;
#	}
#	$count = $count + 1;

   }
    waitpid($pid, 0);
#    if($? == -1){
#	AddValue($stat, {'name'=>"Timed out file name $terminated", 'digest'=>'copy', 'sum'=>'1'});
#	$terminated++;
 #   }else {
	$sucess++;
 #   }
    $count++;

}

print "Total files processed: $count\n";
print "Total files terminated: $terminated\n"; 
print "Total files successeful: $success\n";   
#AddValue($stat, {'name'=>'Completed normally', 'digest'=>'sum', 'content'=>1});

#$statFileAddr = "$arg2/Logs/Stat/job$arg1.xml";
#WriteStat($stat, $statFileAddr);
#print "File is written $statFileAddr\n";

exit 0;
