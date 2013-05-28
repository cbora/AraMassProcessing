#!/usr/bin/perl

#use strict;
use warnings;
use diagnostics;
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

print "*** date ***\n";
print `date`;
print "*** hostname ***\n";
print `hostname`;
print "*** ls -1 ***\n";
print `ls -1`;

# Set up environment variables for ROOT
$ENV{ROOTSYS} = "/net/local/icecube/i3tools/RHEL4-x86_64/root-v5.18.00";
$ENV{PATH} = "$ENV{ROOTSYS}/bin:$ENV{PATH}";
$ENV{LD_LIBRARY_PATH} = "$ENV{ROOTSYS}/lib/root:$ENV{LD_LIBRARY_PATH}";

#system('echo $ROOTSYS');
#system('echo $PATH');
#system('echo $LD_LIBRARY_PATH');

do 'xmlStatistics.pl';	# Module of creating xml-stat files

# Get status of the process with pid
sub ProcessRunning{
	my $pid = @_[0];
	my $status = system("ps $pid > /dev/null");
	if ($status == 0) {
		return 1;
	} else {
		return 0;
	}
}

# Waits for maximum of $maxWait and terminate after that
# If the process is finished normally return 0 otherwise 1
sub WaitAndTerminate {
	my $pid = @_[0];

	print "Waiting for process pid = $pid\n";
	my $waitSec = 1;
	my $numSec = -$waitSec;
	my $maxWait = 10;
	
	while (ProcessRunning($pid)) {
		# The process is still running
		$numSec += $waitSec;
		#print "numSec = $numSec\n";
		if ($numSec > $maxWait) {
			# It is running for too long
			print "Process $pid is timed out. Terminating: " . (system("kill $pid") == 0? 'Done' : 'Failed') . "\n";
			return 1;
		}
		sleep $waitSec;
	}
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

open TASK, "$listFile" or die "Cannot open job-file $listFile : $!";
while (<TASK>) {
	$line = GetFirstLine($_);
	print "\nFile number $count to be processed: $line\n";
	$newFileName = basename($line);
	$now = localtime;
	print "now = $now\n";
	$command = "root -l -q -b \"RootScript.C(\\\"$line\\\",\\\"$arg2/Results/$newFileName\\\")\"";
	$outputLog = "$arg2/Logs/Physics/$newFileName.out";
	$errorLog = "$arg2/Logs/Physics/$newFileName.err";
	# Start process in background and return pid of the new process
	my $executable = "$command 1> $outputLog 2> $errorLog & 
		echo \$!";
#wait \$!
#echo \$?";
	#print "pid = " . `$executable`;
	$results = `$executable`;
#print $results . "\n";
	$pid = GetFirstLine($results);
	print "pid = $pid\n";
#system ("ps $pid");
#sleep 1;

#$status = waitpid(-1, 0);
#print "wait = $status\n";
	if (WaitAndTerminate($pid)) { # Terminated
		AddValue($stat, {'name'=>"Timed out file name $terminated", 'digest'=>'copy', 'content'=>"$line"});
		$terminated ++;
	} else { # Finished
		$success ++;
	}
	$count = $count + 1;
#last;
}
close TASK;
print "Total files processed: $count\n";
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
#print `jobs -l`;
#print "ls\n";
#print `ls ../Data/2012-08-21_15\:38\:24/Results/ -ltr`;
exit 0;
