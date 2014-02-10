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

$arg1 = shift;
$arg2 = shift;
$arg3 = shift;
print "arg1 = $arg1, arg2 = $arg2, arg3 = $arg3\n";

#Unzip files used in workerScript

`tar -zxf archive.tar.gz`;


print "*** date ***\n";
print `date`;
print "*** hostname ***\n";
print `hostname`;
print "*** ls -1 ***\n";
print `ls -1`;


# Set up environment variables for ROOT
#$ENV{ROOTSYS} = "/net/local/icecube/i3tools/RHEL4-x86_64/root-v5.18.00";
#$ENV{PATH} = "$ENV{ROOTSYS}/bin:$ENV{PATH}";
#$ENV{LD_LIBRARY_PATH} = "$ENV{ROOTSYS}/lib/root:$ENV{LD_LIBRARY_PATH}";

#Setting up environment like this does not work for some reason
#setting up environment variables for AraRoot and ROOT by cbora
#$ENV{ROOTSYS} = "/home/jdavies/repositories/root";
#$ENV{PATH} = "$ENV{ROOTSYS}/bin:$ENV{PATH}";
#$ENV{ARA_ROOT_DIR} = "/home/jdavies/repositories/ara/AraRoot/branches/3.3";
#$ENV{ARA_UTIL_INSTALL_DIR} = "/home/jdavies/repositories/install/AraRoot/branches/3.3";
#$ENV{LD_LIBRARY_PATH} = "/home/jdavies/repositories/install/utilities/lib:$ENV{ROOTSYS}/lib:$ENV{ARA_UTIL_INSTALL_DIR}/lib:$ENV{LD_LIBRARY_PATH}";
#-----$ENV{LD_LIBRARY_PATH} = "$ENV{ROOTSYS}/lib:$ENV{LD_LIBRARY_PATH}";
#$ENV{LIBRARY_PATH} = "/home/jdavies/repositories/install/utilities/lib:$ENV{ROOTSYS}/lib/:$ENV{ARA_UTIL_INSTALL_DIR}/lib:$ENV{LD_LIBRARY_PATH}";



#print "Printing environment\n";
#print `env`;

do 'xmlStatistics.pl';	# Module of creating xml-stat files

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
print "*** Running ROOT on each file from the list $listFile ***\n";

my $stat = CreateStat("job$arg1.txt");

my $count = 0;
my $success = 0;
my $nonZeroExitCode = 0;
my $terminated = 0;
#use POSIX ":sys_wait_h";

#To execute Compile.C Root script
	$rootlogon = "root -l -q -b compile.C";
	$run = "$rootlogon & echo \$!";
	print $rootlogon ."\n";
	$done =`$run`;
	print "run rootlogon and compile code if needed\n";  

open TASK, "$listFile" or die "Cannot open job-file $listFile : $!";
while (<TASK>) {
	$line = GetFirstLine($_);
	print "\nFile number $count to be processed: $line\n";
	$newFileName = basename($line);
	$now = localtime;
	print "now = $now\n";
	$processed="Processed_";
	$newFileName=$processed . $newFileName;

        #generating path for non root files
	$outPath=$arg2 . "Results/NonRootOutput/";
	$iIndex=index($newFileName, "un");
	$iIndex= $iIndex + 2;
	$newString= substr($newFileName, $iIndex, 6);
	$newString = "Run" . $newString;
	$newString = $newString . "_";
	$outPath = $outPath . $newString;
	$command = "root -l -q -b \"RootScript.C(\\\"$line\\\",\\\"$arg2/Results/$newFileName\\\", \\\"$outPath\\\")\"";
	$outputLog = "$arg2/Logs/Physics/$newFileName.out";
	$errorLog = "$arg2/Logs/Physics/$newFileName.err";
	# Start process in background and return pid of the new process
	my $executable = "$command 1> $outputLog 2> $errorLog & echo \$!";
	#print "pid = " . `$executable`;
	$results = `$executable`;
        #print $results . "\n";
	$pid = GetFirstLine($results);
	print "pid = $pid\n";
        #system ("ps $pid");
        #sleep 3*60; #sleep for 3minutes
        #$status = waitpid(-1, 0);
        #print "wait = $status\n";
	if (WaitAndTerminate($pid)) { # Terminated
		AddValue($stat, {'name'=>"Timed out file name $terminated", 'digest'=>'copy', 'content'=>"$line"});
		$terminated ++;
	} else { # Finished
		$success ++;
	}
	$count = $count + 1;
        #print `ls -l`;
#last;
}
close TASK;
print "Total files processed: $count\n";
print "Total files terminated: $terminated\n"; 
print "Total files successeful: $success\n";   
AddValue($stat, {'name'=>'Completed normally', 'digest'=>'sum', 'content'=>1});

AddValue($stat, {'name'=>'Total Root files gone through processing', 'digest'=>'sum', 'content'=>$count});
AddValue($stat, {'name'=>'Processed successfully', 'digest'=>'sum', 'content'=>$success});
AddValue($stat, {'name'=>'Killed/timeout of execution', 'digest'=>'sum', 'content'=>$terminated});
AddValue($stat, {'name'=>'Non-zero exit code', 'digest'=>'sum', 'content'=>$nonZeroExitCode});

# During stat digestion this value will add up with 'Total Root files to process'
# and finally will give number of not processed files
AddValue($stat, {'name'=>'Not processed/condor section failure', 'digest'=>'sum', 'content'=>-$count});

$statFileAddr = "$arg2/Logs/Stat/job$arg1.xml";
WriteStat($stat, $statFileAddr);
print "File is written $statFileAddr\n";

exit 0;
